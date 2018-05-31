/*
 * KTOverlappingTrackClustering.cc
 *
 *  Created on: August 7, 2017
 *      Author: C. Claessens
 */

#include "KTOverlappingTrackClustering.hh"

#include "KTLogger.hh"

#include <vector>
#include <cmath>

#ifndef NDEBUG
#include <sstream>
#endif

using std::list;
using std::set;

namespace Katydid
{
    KTLOGGER(otclog, "KTOverlappingTrackClustering");

    KT_REGISTER_PROCESSOR(KTOverlappingTrackClustering, "overlapping-track-clustering");

    KTOverlappingTrackClustering::KTOverlappingTrackClustering(const std::string& name) :
            KTPrimaryProcessor(name),
            fMaxTrackWidth(200000.),
            fCompTracks(),
            fNewTracks(),
            fNewSeqLineCands(),
            fCompSeqLineCands(),
            fNTracks(0),
            fApplyTotalPowerCut(false),
            fApplyAveragePowerCut(false),
            fApplyTotalSNRCut(false),
            fApplyAverageSNRCut(false),
            fApplyTotalUnitlessResidualCut(false),
            fApplyAverageUnitlessResidualCut(false),
            fTotalPowerThreshold(0.0),
            fAveragePowerThreshold(0.0),
            fTotalSNRThreshold(0.0),
            fAverageSNRThreshold(0.0),
            fTotalUnitlessResidualThreshold(0.0),
            fAverageUnitlessResidualThreshold(0.0),
            fTrackSignal("track", this),
            fSeqLineCandSignal("sql-cand", this),
            fDoneSignal("clustering-done", this),
            fTakeTrackSlot("track", this, &KTOverlappingTrackClustering::TakeTrack),
            fTakeSeqLineCandSlot("sql-cand", this, &KTOverlappingTrackClustering::TakeSeqLineCandidate)
    {
        RegisterSlot("do-clustering", this, &KTOverlappingTrackClustering::DoClusteringSlot);
    }

    KTOverlappingTrackClustering::~KTOverlappingTrackClustering()
    {
    }

    bool KTOverlappingTrackClustering::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (node->has("max-track-width"))
        {
            SetMaxTrackWidth(node->get_value<double>("max-track-width"));
        }
        if (node->has("apply-power-cut"))
        {
            SetApplyTotalPowerCut(node->get_value("apply-total-power-cut", GetApplyTotalPowerCut()));
            SetTotalPowerThreshold(node->get_value("power-total-threshold", GetTotalPowerThreshold()));
        }
        if (node->has("apply-power-density-cut"))
        {
            SetApplyAveragePowerCut(node->get_value("apply-average-power-cut", GetApplyAveragePowerCut()));
            SetAveragePowerThreshold(node->get_value("power-average-power-threshold", GetAveragePowerThreshold()));
        }
        if (node->has("apply-total-snr-cut"))
        {
            SetApplyTotalSNRCut(node->get_value("apply-total-snr-cut", GetApplyTotalSNRCut()));
            SetTotalSNRThreshold(node->get_value("total-snr-threshold", GetTotalSNRThreshold()));
        }
        if (node->has("apply-average-snr-cut"))
        {
            SetApplyAverageSNRCut(node->get_value("apply-average-snr-cut", GetApplyAverageSNRCut()));
            SetAverageSNRThreshold(node->get_value("average-snr-threshold", GetAverageSNRThreshold()));
        }
        if (node->has("apply-total-unitless-residual-cut"))
        {
            SetApplyTotalUnitlessResidualCut(node->get_value("apply-total-unitless-residual-cut", GetApplyTotalUnitlessResidualCut()));
            SetTotalUnitlessResidualThreshold(node->get_value("total-unitless-residual-threshold", GetTotalUnitlessResidualThreshold()));
        }
        if (node->has("apply-average-unitless-residual-cut"))
        {
            SetApplyAverageUnitlessResidualCut(node->get_value("apply-average-unitless-residual-cut", GetApplyAverageUnitlessResidualCut()));
            SetAverageUnitlessResidualThreshold(node->get_value("average-unitless-residual-threshold", GetAverageUnitlessResidualThreshold()));
        }
        return true;
    }

    bool KTOverlappingTrackClustering::TakeTrack(KTProcessedTrackData& track)
    {
        // ignore the track if it's been cut
        if (track.GetIsCut()) return true;


        KTDEBUG(otclog, "Taking track: (" << track.GetStartTimeInRunC() << ", " << track.GetStartFrequency() << ", " << track.GetEndTimeInRunC() << ", " << track.GetEndFrequency() << ")");

        // copy the full track data
        fCompTracks.push_back(track);

        return true;
    }
    bool KTOverlappingTrackClustering::TakeSeqLineCandidate(KTSequentialLineData& SeqLineCand)
    {

        KTDEBUG(otclog, "Taking SeqLine candidate: (" << SeqLineCand.GetStartTimeInRunC() << ", " << SeqLineCand.GetStartFrequency() << ", " << SeqLineCand.GetEndTimeInRunC() << ", " << SeqLineCand.GetEndFrequency() << ")");

        // copy the full track data
        fCompSeqLineCands.push_back(SeqLineCand);

        return true;
    }

    void KTOverlappingTrackClustering::DoClusteringSlot()
    {
        if (! Run())
        {
            KTERROR(otclog, "An error occurred while running the clustering");
        }
        return;
    }

    bool KTOverlappingTrackClustering::Run()
    {
        if (fCompTracks.size() != 0 )
        {
            KTINFO( otclog, "Clustering procTracks");
            return DoTrackClustering();
        }
        else
        {
            KTINFO( otclog, "Clustering SeqLine Candidates");
            return DoSeqLineClustering();
        }
    }

    bool KTOverlappingTrackClustering::DoTrackClustering()
    {
        if (! FindMatchingTracks())
        {
            KTERROR(otclog, "An error occurred while identifying overlapping tracks");
            return false;
        }

        KTDEBUG(otclog, "Track clustering complete");
        fDoneSignal();

        return true;
    }
    bool KTOverlappingTrackClustering::DoSeqLineClustering()
        {
            if (! FindMatchingSeqLineCands())
            {
                KTERROR(otclog, "An error occurred while identifying overlapping SeqLine candidates");
                return false;
            }

            KTDEBUG(otclog, "SeqLine clustering complete");
            fDoneSignal();

            return true;
        }

    bool KTOverlappingTrackClustering::FindMatchingTracks()
    {
        KTINFO(otclog, "Finding overlapping tracks");
        KTDEBUG(otclog, "FrequencyAcceptance is: "<<fMaxTrackWidth);
        fNewTracks.clear();

        unsigned numberOfTracks = fCompTracks.size();
        unsigned numberOfNewTracks = fNewTracks.size();



        if (numberOfTracks > 1)
        {
            while (numberOfTracks!=numberOfNewTracks)
            {
                numberOfTracks = fCompTracks.size();
                KTDEBUG(otclog, "Number of tracks to cluster: "<< numberOfTracks);
                this->OverlapClustering();

                // Update number of tracks
                numberOfNewTracks = fNewTracks.size();

                KTDEBUG(otclog, "Number of new tracks: "<< numberOfNewTracks);

                fCompTracks.clear();
                fCompTracks = fNewTracks;
                fNewTracks.clear();
            }
        }

        this->EmitTrackCandidates();

        return true;
    }
    bool KTOverlappingTrackClustering::FindMatchingSeqLineCands()
    {
        KTINFO( otclog, "Finding overlapping tracks" );
        KTDEBUG( otclog, "FrequencyAcceptance is: "<<fMaxTrackWidth );
        fNewSeqLineCands.clear();

        unsigned numberOfCands = fCompSeqLineCands.size();
        unsigned numberOfNewCands = fNewSeqLineCands.size();



        if (numberOfCands > 1)
        {
            while (numberOfCands!=numberOfNewCands)
            {
                numberOfCands = fCompSeqLineCands.size();
                KTDEBUG(otclog, "Number of SeqLine candidates to cluster: "<< numberOfCands);
                this->OverlapSeqLineClustering();

                // Update number of tracks
                numberOfNewCands = fNewSeqLineCands.size();

                KTDEBUG(otclog, "Number of new SeqLine candidates: "<< numberOfNewCands);

                fCompSeqLineCands.clear();
                fCompSeqLineCands = fNewSeqLineCands;
                fNewSeqLineCands.clear();
            }
        }

        this->EmitSeqLineCandidates();

        return true;
    }

    bool KTOverlappingTrackClustering::OverlapClustering()
    {
        bool match = false;
        for (std::vector<KTProcessedTrackData>::iterator compIt = fCompTracks.begin(); compIt != fCompTracks.end(); ++compIt)
        {
            match = false;
            for (std::vector<KTProcessedTrackData>::iterator newIt = fNewTracks.begin(); newIt != fNewTracks.end(); ++newIt)
            {
                if (this->DoTheyOverlap(*compIt, *newIt))
                {
                    match = true;
                    KTDEBUG(otclog, "Found overlapping tracks")
                    this->CombineTracks(*compIt, *newIt);
                    break;
                }

                // since there appear to be real tracks crossing each other this check is removed here
                /*if (this->DoTheyCross(*compIt, *newIt))
                {
                    match = true;
                    KTDEBUG(otclog, "Found crossing tracks")
                    this->CombineTracks(*compIt, *newIt);
                    break;
                }*/
            }

            if (match == false)
            {
                KTProcessedTrackData newTrack(*compIt);
                fNewTracks.push_back(newTrack);
            }
        }
        return true;
    }
    bool KTOverlappingTrackClustering::OverlapSeqLineClustering()
    {
        bool match = false;
        for (std::vector<KTSequentialLineData>::iterator compIt = fCompSeqLineCands.begin(); compIt != fCompSeqLineCands.end(); ++compIt)
        {
            match = false;
            for (std::vector<KTSequentialLineData>::iterator newIt = fNewSeqLineCands.begin(); newIt != fNewSeqLineCands.end(); ++newIt)
            {
                if (this->DoTheyOverlap(*compIt, *newIt))
                {
                    match = true;
                    KTDEBUG(otclog, "Found overlapping tracks")
                    this->CombineSeqLineCandidates(*compIt, *newIt);
                    break;
                }
            }

            if (match == false)
            {
                KTSequentialLineData newSeqLineCand(*compIt);
                fNewSeqLineCands.push_back(newSeqLineCand);
            }
        }
        return true;
    }


    const void KTOverlappingTrackClustering::CombineTracks(const KTProcessedTrackData& oldTrack, KTProcessedTrackData& newTrack)
    {
        if (oldTrack.GetStartTimeInRunC() < newTrack.GetStartTimeInRunC())
        {
            newTrack.SetStartTimeInRunC( oldTrack.GetStartTimeInRunC());
            newTrack.SetStartTimeInAcq( oldTrack.GetStartTimeInAcq());
            newTrack.SetStartFrequency( oldTrack.GetStartFrequency());
            newTrack.SetStartTimeInRunCSigma( oldTrack.GetStartTimeInRunCSigma());
            newTrack.SetStartFrequencySigma( oldTrack.GetStartFrequencySigma());
            newTrack.SetSlope( (newTrack.GetEndFrequency() - newTrack.GetStartFrequency())/(newTrack.GetEndTimeInRunC() - newTrack.GetStartTimeInRunC()));

        }
        if (oldTrack.GetEndTimeInRunC() > newTrack.GetEndTimeInRunC())
        {
            newTrack.SetEndTimeInRunC( oldTrack.GetEndTimeInRunC());
            newTrack.SetEndFrequency( oldTrack.GetEndFrequency());
            newTrack.SetEndTimeInRunCSigma( oldTrack.GetEndTimeInRunCSigma());
            newTrack.SetEndFrequencySigma( oldTrack.GetEndFrequencySigma());
            newTrack.SetSlope( (newTrack.GetEndFrequency() - newTrack.GetStartFrequency())/(newTrack.GetEndTimeInRunC() - newTrack.GetStartTimeInRunC()));

        }
        newTrack.SetTotalPower( newTrack.GetTotalPower() + oldTrack.GetTotalPower());
        newTrack.SetTotalPowerSigma( sqrt(newTrack.GetTotalPowerSigma()*newTrack.GetTotalPowerSigma() + oldTrack.GetTotalPowerSigma()*oldTrack.GetTotalPowerSigma()) );
        //newTrack.SetTimeLength( newTrack.GetEndTimeInRunC() - newTrack.GetStartTimeInRunC());
        newTrack.SetTimeLengthSigma( sqrt(newTrack.GetTimeLengthSigma()*newTrack.GetTimeLengthSigma() + oldTrack.GetTimeLengthSigma()*oldTrack.GetTimeLengthSigma()) );
        //newTrack.SetFrequencyWidth( newTrack.GetEndFrequency() - newTrack.GetStartFrequency() );
        newTrack.SetFrequencyWidthSigma( sqrt(newTrack.GetFrequencyWidthSigma()*newTrack.GetFrequencyWidthSigma() + oldTrack.GetFrequencyWidthSigma()*oldTrack.GetFrequencyWidthSigma()) );
        newTrack.SetSlopeSigma( sqrt(newTrack.GetSlopeSigma()*newTrack.GetSlopeSigma() + oldTrack.GetSlopeSigma()*oldTrack.GetSlopeSigma()));
        newTrack.SetInterceptSigma( sqrt(newTrack.GetInterceptSigma()*newTrack.GetInterceptSigma() + oldTrack.GetInterceptSigma()*oldTrack.GetInterceptSigma()));
    }


    const void KTOverlappingTrackClustering::CombineSeqLineCandidates(const KTSequentialLineData& oldSeqLineCand, KTSequentialLineData& newSeqLineCand)
    {
        if (oldSeqLineCand.GetStartTimeInRunC() < newSeqLineCand.GetStartTimeInRunC())
        {
            newSeqLineCand.SetComponent( oldSeqLineCand.GetComponent() );
            newSeqLineCand.SetAcquisitionID( oldSeqLineCand.GetAcquisitionID() );
            newSeqLineCand.SetStartTimeInRunC( oldSeqLineCand.GetStartTimeInRunC() );
            newSeqLineCand.SetStartTimeInAcq( oldSeqLineCand.GetStartTimeInAcq() );
            newSeqLineCand.SetStartFrequency( oldSeqLineCand.GetStartFrequency() );
        }
        if (oldSeqLineCand.GetEndTimeInRunC() > newSeqLineCand.GetEndTimeInRunC())
        {
            newSeqLineCand.SetEndTimeInRunC( oldSeqLineCand.GetEndTimeInRunC());
            newSeqLineCand.SetEndFrequency( oldSeqLineCand.GetEndFrequency());
        }
        newSeqLineCand.SetSlope( (newSeqLineCand.GetEndFrequency() - newSeqLineCand.GetStartFrequency())/(newSeqLineCand.GetEndTimeInRunC() - newSeqLineCand.GetStartTimeInRunC()) );

        KTDiscriminatedPoints points = oldSeqLineCand.GetPoints();
        for(KTDiscriminatedPoints::const_iterator pointIt = points.begin(); pointIt != points.end(); ++pointIt )
        {
            //KTDEBUG( otclog, "Adding points from oldSeqLineCand to newSeqLineCand: "<<pointIt->fTimeInRunC<<" "<<pointIt->fFrequency<<" "<<pointIt->fAmplitude<<" "<<pointIt->fAmplitude );
            newSeqLineCand.AddPoint(*pointIt);
        }
    }


    bool KTOverlappingTrackClustering::DoTheyOverlap(KTProcessedTrackData& track1, KTProcessedTrackData& track2)
    {
        // if there are two tracks that should be just one, any point of the track will be close to the other track (extrapolated)
        // therefore it is enough to check start and endpoint of a track. one should overlap in time, both should be close in frequency

        // if the start time of track 2 is between start and end time of track 1
        bool condition1 = track2.GetStartTimeInRunC() < track1.GetEndTimeInRunC() and track2.GetStartTimeInRunC() >= track1.GetStartTimeInRunC();

        // and the start frequency of track 2 is too close to track 1
        bool condition2 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;

        // same for endpoint
        bool condition3 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth*5.0;

        if (condition1 and condition2 and condition3)
        {
            return true;
        }
        // the other way around
        bool condition4 = track1.GetStartTimeInRunC() < track2.GetEndTimeInRunC() and track1.GetStartTimeInRunC() >= track2.GetStartTimeInRunC();
        bool condition5 = std::abs(track1.GetStartFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth;
        bool condition6 = std::abs(track1.GetEndFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth*5.0;

        if (condition4 and condition5 and condition6)
        {
            return true;
        }
        // same for end point of track2
        condition1 = track2.GetEndTimeInRunC() <= track1.GetEndTimeInRunC() and track2.GetEndTimeInRunC() > track1.GetStartTimeInRunC();
        condition2 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;
        condition3 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth*5.0;

        if (condition1 and condition2 and condition3)
        {
            return true;
        }
        // the other way around
        condition4 = track1.GetEndTimeInRunC() <= track2.GetEndTimeInRunC() and track1.GetEndTimeInRunC() > track2.GetStartTimeInRunC();
        condition5 = std::abs(track1.GetEndFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth;
        condition6 = std::abs(track1.GetStartFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth*5.0;

        if (condition4 and condition5 and condition6)
        {
            return true;
        }

        return false;
    }
    bool KTOverlappingTrackClustering::DoTheyOverlap(KTSequentialLineData& track1, KTSequentialLineData& track2)
    {
        // if there are two tracks that should be just one, any point of the track will be close to the other track (extrapolated)
        // therefore it is enough to check start and endpoint of a track. one should overlap in time, both should be close in frequency

        // if the start time of track 2 is between start and end time of track 1
        bool condition1 = track2.GetStartTimeInRunC() < track1.GetEndTimeInRunC() and track2.GetStartTimeInRunC() >= track1.GetStartTimeInRunC();

        // and the start frequency of track 2 is too close to track 1
        bool condition2 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;

        // same for endpoint
        bool condition3 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth*5.0;

        if (condition1 and condition2 and condition3)
        {
            return true;
        }
        // the other way around
        bool condition4 = track1.GetStartTimeInRunC() < track2.GetEndTimeInRunC() and track1.GetStartTimeInRunC() >= track2.GetStartTimeInRunC();
        bool condition5 = std::abs(track1.GetStartFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth;
        bool condition6 = std::abs(track1.GetEndFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth*5.0;

        if (condition4 and condition5 and condition6)
        {
            return true;
        }
        // same for end point of track2
        condition1 = track2.GetEndTimeInRunC() <= track1.GetEndTimeInRunC() and track2.GetEndTimeInRunC() > track1.GetStartTimeInRunC();
        condition2 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;
        condition3 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth*5.0;

        if (condition1 and condition2 and condition3)
        {
            return true;
        }
        // the other way around
        condition4 = track1.GetEndTimeInRunC() <= track2.GetEndTimeInRunC() and track1.GetEndTimeInRunC() > track2.GetStartTimeInRunC();
        condition5 = std::abs(track1.GetEndFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth;
        condition6 = std::abs(track1.GetStartFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth*5.0;

        if (condition4 and condition5 and condition6)
        {
            return true;
        }

        return false;
    }
    /*
    bool KTOverlappingTrackClustering::DoTheyCross(KTProcessedTrackData& track1, KTProcessedTrackData& track2)
    {
        // if the start time or the end time of track 2 is between start and end time of track 1
        bool condition1 = track2.GetStartTimeInRunC() < track1.GetEndTimeInRunC() and track2.GetStartTimeInRunC() > track1.GetStartTimeInRunC();
        bool condition2 = track2.GetEndTimeInRunC() < track1.GetEndTimeInRunC() and track2.GetEndTimeInRunC() > track1.GetStartTimeInRunC();

        // and they cross in frequency this way
        bool cross_condition1 = track2.GetStartFrequency() < (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()));
        bool cross_condition2 = track2.GetEndFrequency() > (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()));

        // or the other way around
        bool cross_condition3 = track2.GetStartFrequency() > (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()));
        bool cross_condition4 = track2.GetEndFrequency() < (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()));


        if ((condition1 or condition2) and ((cross_condition1 and cross_condition2) or (cross_condition3 and cross_condition4)))
        {
            return true;
        }

        // the same as above but for the inverted track combination
        condition1 = track1.GetStartTimeInRunC() < track2.GetEndTimeInRunC() and track1.GetStartTimeInRunC() > track2.GetStartTimeInRunC();
        condition2 = track1.GetEndTimeInRunC() < track2.GetEndTimeInRunC() and track1.GetEndTimeInRunC() > track2.GetStartTimeInRunC();
   
        // and they cross in frequency this way
        cross_condition1 = track1.GetStartFrequency() < (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()));
        cross_condition2 = track1.GetEndFrequency() > (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()));
        
        // or the other way around
        cross_condition3 = track1.GetStartFrequency() > (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()));
        cross_condition4 = track1.GetEndFrequency() < (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()));
   
   
        if ((condition1 or condition2) and ((cross_condition1 and cross_condition2) or (cross_condition3 and cross_condition4)))
        {   
            return true;
        }


        return false;
    }*/

    void KTOverlappingTrackClustering::EmitTrackCandidates()
    {
        KTDEBUG(otclog, "Number of tracks to emit: "<<fCompTracks.size());
        KTINFO(otclog, "Clustering done.");

        std::vector<KTProcessedTrackData>::iterator trackIt = fCompTracks.begin();

        while(trackIt!=fCompTracks.end())
        {
            bool emitThisCandidate = true;

            if (fApplyTotalPowerCut)
            {
                if (trackIt->GetTotalPower() <= fTotalPowerThreshold)
                {
                    KTDEBUG(otclog, "track power below threshold: "<<trackIt->GetTotalPower()<<" "<<fTotalPowerThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAveragePowerCut)
            {
                if (trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <= fAveragePowerThreshold)
                {
                    KTDEBUG(otclog, "average track power below threshold: "<<trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <<" "<< fAveragePowerThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyTotalSNRCut)
            {
                if (trackIt->GetTotalPower() <= fTotalSNRThreshold)
                {
                    KTDEBUG(otclog, "total track snr below threshold: "<<trackIt->GetTotalPower()<<" "<<fTotalSNRThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAverageSNRCut)
            {
                if (trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <= fAverageSNRThreshold)
                {
                    KTDEBUG(otclog, "average track snr below threshold: "<<trackIt->GetTotalPower()<<" "<<fAverageSNRThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyTotalUnitlessResidualCut)
            {
                if (trackIt->GetTotalPower() <= fTotalUnitlessResidualThreshold)
                {
                    KTDEBUG(otclog, "total track residuals below threshold: "<<trackIt->GetTotalPower()<<" "<<fTotalUnitlessResidualThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAverageUnitlessResidualCut)
            {
                if (trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <= fAverageUnitlessResidualThreshold)
                {
                    KTDEBUG(otclog, "average track residuals below threshold: "<<trackIt->GetTotalPower()<<" "<<fAverageUnitlessResidualThreshold);
                    emitThisCandidate = false;
                }
            }

            if (emitThisCandidate == true)
            {
                // Set up new data object
                Nymph::KTDataPtr data( new Nymph::KTData() );
                KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
                newTrack.SetComponent( trackIt->GetComponent() );
                newTrack.SetAcquisitionID( trackIt->GetAcquisitionID() );
                newTrack.SetTrackID( fNTracks );
                fNTracks++;

                newTrack.SetStartTimeInRunC( trackIt->GetStartTimeInRunC() );
                newTrack.SetStartTimeInRunCSigma( trackIt->GetStartTimeInRunCSigma() );
                newTrack.SetEndTimeInRunC( trackIt->GetEndTimeInRunC() );
                newTrack.SetEndTimeInRunCSigma( trackIt->GetEndTimeInRunCSigma() );
                newTrack.SetStartTimeInAcq( trackIt->GetStartTimeInAcq() );
                newTrack.SetTimeLength( trackIt->GetTimeLength() );
                newTrack.SetTimeLengthSigma( trackIt->GetTimeLengthSigma() );
                newTrack.SetStartFrequency( trackIt->GetStartFrequency() );
                newTrack.SetStartFrequency( trackIt->GetStartFrequencySigma() );
                newTrack.SetEndFrequency( trackIt->GetEndFrequency() );
                newTrack.SetEndFrequencySigma( trackIt->GetEndFrequencySigma() );
                newTrack.SetSlope( trackIt->GetSlope() );
                newTrack.SetSlopeSigma( trackIt->GetSlopeSigma() );
                newTrack.SetTotalPower( trackIt->GetTotalPower() );
                newTrack.SetTotalPowerSigma( trackIt->GetTotalPowerSigma() );
                newTrack.SetFrequencyWidth( trackIt->GetFrequencyWidth() );
                newTrack.SetFrequencyWidthSigma( trackIt->GetFrequencyWidthSigma() );
                newTrack.SetIntercept( trackIt->GetFrequencyWidth() );
                newTrack.SetInterceptSigma( trackIt->GetInterceptSigma() );


                // Process & emit new track

                KTINFO(otclog, "Now processing tracksCandidates");
                ProcessNewTrack( newTrack );

                KTDEBUG(otclog, "Emitting track signal");
                fCandidates.insert( data );
                fTrackSignal( data );
            }
            trackIt = fCompTracks.erase(trackIt);
        }
    }

    void KTOverlappingTrackClustering::EmitSeqLineCandidates()
    {
        KTDEBUG(otclog, "Number of tracks to emit: "<<fCompSeqLineCands.size());
        KTINFO(otclog, "Clustering done.");

        std::vector<KTSequentialLineData>::iterator candIt = fCompSeqLineCands.begin();

        while( candIt!=fCompSeqLineCands.end() )
        {
            bool emitThisCandidate = true;
            double summedPower = 0.0;
            double summedSNR = 0.0;
            double summedUnitlessResidual = 0.0;

            KTDiscriminatedPoints& points = candIt->GetPoints();
            for (KTDiscriminatedPoints::const_iterator pointIt = points.begin(); pointIt != points.end(); ++pointIt )
            {
                summedPower += pointIt->fNeighborhoodAmplitude;
                summedSNR += pointIt->fNeighborhoodAmplitude / pointIt->fMean;
                summedUnitlessResidual += ( pointIt->fNeighborhoodAmplitude - pointIt->fMean ) / pointIt->fVariance;
            }

            if (fApplyTotalPowerCut)
            {
                if (summedPower <= fTotalPowerThreshold)
                {
                    KTDEBUG(otclog, "track power below threshold: "<<summedPower<<" "<<fTotalPowerThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAveragePowerCut)
            {
                if (summedPower/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC()) <= fAveragePowerThreshold)
                {
                    KTDEBUG(otclog, "average track power below threshold: "<<summedPower/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC()) <<" "<< fAveragePowerThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyTotalSNRCut)
            {
                if (summedSNR <= fTotalSNRThreshold)
                {
                    KTDEBUG(otclog, "total track snr below threshold: "<<summedSNR<<" "<<fTotalSNRThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAverageSNRCut)
            {
                if (summedSNR/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC()) <= fAverageSNRThreshold)
                {
                    KTDEBUG(otclog, "average track snr below threshold: "<<summedSNR/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC())<<" "<<fAverageSNRThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyTotalUnitlessResidualCut)
            {
                if (summedUnitlessResidual <= fTotalUnitlessResidualThreshold)
                {
                    KTDEBUG(otclog, "total track residuals below threshold: "<<summedUnitlessResidual<<" "<<fTotalUnitlessResidualThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAverageUnitlessResidualCut)
            {
                if (summedUnitlessResidual/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC()) <= fAverageUnitlessResidualThreshold)
                {
                    KTDEBUG(otclog, "average track residuals below threshold: "<<summedUnitlessResidual/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC())<<" "<<fAverageUnitlessResidualThreshold);
                    emitThisCandidate = false;
                }
            }
            if (emitThisCandidate == true )
            {

                // Set up new data object
                Nymph::KTDataPtr data( new Nymph::KTData() );
                KTSequentialLineData& newSeqLineCand = data->Of< KTSequentialLineData >();
                newSeqLineCand.SetComponent( candIt->GetComponent() );
                newSeqLineCand.SetAcquisitionID( candIt->GetAcquisitionID() );
                newSeqLineCand.SetCandidateID( fNTracks );
                fNTracks++;

                newSeqLineCand.SetSlope(candIt->GetSlope());

                KTDiscriminatedPoints& points = candIt->GetPoints();
                for(KTDiscriminatedPoints::const_iterator pointIt = points.begin(); pointIt != points.end(); ++pointIt )
                {
                    //KTDEBUG( otclog, "Adding points to newSeqLineCand: "<<pointIt->fTimeInRunC<<" "<<pointIt->fFrequency<<" "<<pointIt->fAmplitude<<" "<<pointIt->fNeighborhoodAmplitude );
                    newSeqLineCand.AddPoint( *pointIt );
                }
                KTDEBUG( otclog, "Emitting SeqLine signal" );
                fCandidates.insert( data );
                fSeqLineCandSignal( data );
            }
        candIt = fCompSeqLineCands.erase(candIt);
        }
    }

    const void KTOverlappingTrackClustering::ProcessNewTrack( KTProcessedTrackData& myNewTrack )
    {
        myNewTrack.SetTimeLength( myNewTrack.GetEndTimeInRunC() - myNewTrack.GetStartTimeInRunC() );
        myNewTrack.SetFrequencyWidth( myNewTrack.GetEndFrequency() - myNewTrack.GetStartFrequency() );
        myNewTrack.SetSlope( myNewTrack.GetFrequencyWidth() / myNewTrack.GetTimeLength() );
        myNewTrack.SetIntercept( myNewTrack.GetStartFrequency() - myNewTrack.GetSlope() * myNewTrack.GetStartTimeInRunC() );
    }


} /* namespace Katydid */
