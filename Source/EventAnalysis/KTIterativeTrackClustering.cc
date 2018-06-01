/*
 * KTIterativeTrackClustering.cc
 *
 *  Created on: August 7, 2017
 *      Author: C. Claessens
 */

#include "KTIterativeTrackClustering.hh"
#include "KTLogger.hh"

#include <vector>
#include <cmath>

#ifndef NDEBUG
#include <sstream>
#endif



namespace Katydid
{
    KTLOGGER(itclog, "KTIterativeTrackClustering");

    KT_REGISTER_PROCESSOR(KTIterativeTrackClustering, "iterative-track-clustering");

    KTIterativeTrackClustering::KTIterativeTrackClustering(const std::string& name) :
            KTPrimaryProcessor(name),
            fTimeGapTolerance(0.005),
            fFrequencyAcceptance(185000.0),
            fMaxTrackWidth(50000.0),
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
            fTakeTrackSlot("track", this, &KTIterativeTrackClustering::TakeTrack),
            fTakeSeqLineCandSlot("sql-cand", this, &KTIterativeTrackClustering::TakeSeqLineCandidate)
    {
        RegisterSlot("do-clustering", this, &KTIterativeTrackClustering::DoClusteringSlot);
    }

    KTIterativeTrackClustering::~KTIterativeTrackClustering()
    {
    }

    bool KTIterativeTrackClustering::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetTimeGapTolerance(node->get_value("time-gap-tolerance", GetTimeGapTolerance()));
        SetFrequencyAcceptance(node->get_value("frequency-acceptance", GetFrequencyAcceptance()));

        if (node->has("max-track-width"))
        {
            SetMaxTrackWidth(node->get_value<double>("max-track-width"));
        }
        if (node->has("apply-power-cut"))
        {
            SetApplyTotalPowerCut(node->get_value("apply-total-power-cut", GetApplyTotalPowerCut()));
            SetTotalPowerThreshold(node->get_value("total-power-threshold", GetTotalPowerThreshold()));
        }
        if (node->has("apply-power-density-cut"))
        {
            SetApplyAveragePowerCut(node->get_value("apply-average-power-cut", GetApplyAveragePowerCut()));
            SetAveragePowerThreshold(node->get_value("average-power-threshold", GetAveragePowerThreshold()));
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

    bool KTIterativeTrackClustering::TakeTrack(KTProcessedTrackData& track)
    {
        // ignore the track if it's been cut
        if (track.GetIsCut()) return true;


        KTDEBUG(itclog, "Taking track: (" << track.GetStartTimeInRunC() << ", " << track.GetStartFrequency() << ", " << track.GetEndTimeInRunC() << ", " << track.GetEndFrequency() << ")");

        // copy the full track data
        fCompTracks.push_back(track);

        return true;
    }
    bool KTIterativeTrackClustering::TakeSeqLineCandidate(KTSequentialLineData& SeqLineCand)
    {

        KTDEBUG(itclog, "Taking SeqLine candidate: (" << SeqLineCand.GetStartTimeInRunC() << ", " << SeqLineCand.GetStartFrequency() << ", " << SeqLineCand.GetEndTimeInRunC() << ", " << SeqLineCand.GetEndFrequency() << ")");

        // copy the full track data
        fCompSeqLineCands.push_back(SeqLineCand);

        return true;
    }
    void KTIterativeTrackClustering::DoClusteringSlot()
    {
        if (! Run())
        {
            KTERROR(itclog, "An error occurred while running the iterative track clustering");
        }
        return;
    }

    bool KTIterativeTrackClustering::Run()
    {
        if (fCompTracks.size() != 0 )
        {
            KTINFO( itclog, "Clustering procTracks");
            return DoTrackClustering();
        }
        else
        {
            KTINFO( itclog, "Clustering SeqLine Candidates");
            return DoSeqLineClustering();
        }
    }

    bool KTIterativeTrackClustering::DoTrackClustering()
    {
        if (! FindMatchingTracks())
        {
            KTERROR(itclog, "An error occurred while identifying extrapolated tracks");
            return false;
        }

        KTDEBUG(itclog, "Track building complete");
        fDoneSignal();

        return true;
    }
    bool KTIterativeTrackClustering::DoSeqLineClustering()
        {
            if (! FindMatchingSeqLineCands())
            {
                KTERROR(itclog, "An error occurred while identifying overlapping SeqLine candidates");
                return false;
            }

            KTDEBUG(itclog, "SeqLine clustering complete");
            fDoneSignal();

            return true;
        }
    bool KTIterativeTrackClustering::FindMatchingTracks()
    {
        KTINFO(itclog, "Finding extrapolated tracks");
        KTDEBUG(itclog, "TimeGapTolerance FrequencyAcceptance and MaxTrackWidth are: "<<fTimeGapTolerance<< " "<<fFrequencyAcceptance<< " "<<fMaxTrackWidth);
        fNewTracks.clear();

        unsigned numberOfTracks = fCompTracks.size();
        unsigned numberOfNewTracks = fNewTracks.size();


        if (numberOfTracks > 1)
        {
            while (numberOfTracks!=numberOfNewTracks)
            {
                numberOfTracks = fCompTracks.size();
                KTDEBUG(itclog, "Number of tracks to cluster: "<< numberOfTracks);
                this->ExtrapolateTrackClustering();

                // Update number of tracks
                numberOfNewTracks = fNewTracks.size();

                KTDEBUG(itclog, "Number of new tracks: "<< numberOfNewTracks);

                fCompTracks.clear();
                fCompTracks = fNewTracks;
                fNewTracks.clear();
            }
        }

        this->EmitTrackCandidates();

        return true;
    }
    bool KTIterativeTrackClustering::FindMatchingSeqLineCands()
    {
        KTINFO(itclog, "Finding extrapolated SeqLine cands");
        KTDEBUG(itclog, "TimeGapTolerance FrequencyAcceptance and MaxTrackWidth are: "<<fTimeGapTolerance<< " "<<fFrequencyAcceptance<< " "<<fMaxTrackWidth);
        fNewSeqLineCands.clear();

        unsigned numberOfSeqLineCands = fCompSeqLineCands.size();
        unsigned numberOfNewSeqLineCands = fNewSeqLineCands.size();


        if (numberOfSeqLineCands > 1)
        {
            while (numberOfSeqLineCands!=numberOfNewSeqLineCands)
            {
                numberOfSeqLineCands = fCompSeqLineCands.size();
                KTDEBUG(itclog, "Number of SeqLineCands to cluster: "<< numberOfSeqLineCands);
                this->ExtrapolateSeqLineClustering();

                // Update number of tracks
                numberOfNewSeqLineCands = fNewSeqLineCands.size();

                KTDEBUG(itclog, "Number of new SeqLineCands: "<< numberOfNewSeqLineCands);

                fCompSeqLineCands.clear();
                fCompSeqLineCands = fNewSeqLineCands;
                fNewSeqLineCands.clear();
            }
        }

        this->EmitSeqLineCandidates();

        return true;
    }


    bool KTIterativeTrackClustering::ExtrapolateTrackClustering()
    {
        bool match = false;
        for (std::vector<KTProcessedTrackData>::iterator compIt = fCompTracks.begin(); compIt != fCompTracks.end(); ++compIt)
        {
            match = false;
            for (std::vector<KTProcessedTrackData>::iterator newIt = fNewTracks.begin(); newIt != fNewTracks.end(); ++newIt)
            {
                if (this->DoTheyMatch(*compIt, *newIt))
                {
                    match = true;
                    KTDEBUG(itclog, "Found matching tracks");
                    this->CombineTracks(*compIt, *newIt);
                    break;
                }
                // it is possible that the segments that get combined first are not direct neighbors in time
                // in that case there can be a track segment very close to an already combined track
                if (this->DoTheyOverlap(*compIt, *newIt))
                {
                    match = true;
                    KTDEBUG(itclog, "Found overlapping tracks");
                    this->CombineTracks(*compIt, *newIt);
                    break;   
                }
            }

            if (match == false)
            {
                KTProcessedTrackData newTrack(*compIt);
                fNewTracks.push_back(newTrack);
            }   
        }
        return true;
    }
    bool KTIterativeTrackClustering::ExtrapolateSeqLineClustering()
    {
        bool match = false;
        for (std::vector<KTSequentialLineData>::iterator compIt = fCompSeqLineCands.begin(); compIt != fCompSeqLineCands.end(); ++compIt)
        {
            match = false;
            for (std::vector<KTSequentialLineData>::iterator newIt = fNewSeqLineCands.begin(); newIt != fNewSeqLineCands.end(); ++newIt)
            {
                if (this->DoTheyMatch(*compIt, *newIt))
                {
                    match = true;
                    KTDEBUG(itclog, "Found matching SeqLineCandidates");
                    this->CombineSeqLineCandidates(*compIt, *newIt);
                    break;
                }
                // it is possible that the segments that get combined first are not direct neighbors in time
                // in that case there can be a track segment very close to an already combined track
                if (this->DoTheyOverlap(*compIt, *newIt))
                {
                    match = true;
                    KTDEBUG(itclog, "Found overlapping SeqLineCandidates");
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

    const void KTIterativeTrackClustering::CombineTracks(const KTProcessedTrackData& oldTrack, KTProcessedTrackData& newTrack)
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
    const void KTIterativeTrackClustering::CombineSeqLineCandidates(const KTSequentialLineData& oldSeqLineCand, KTSequentialLineData& newSeqLineCand)
    {
        if (oldSeqLineCand.GetStartTimeInRunC() < newSeqLineCand.GetStartTimeInRunC())
        {
            newSeqLineCand.SetStartTimeInRunC( oldSeqLineCand.GetStartTimeInRunC() );
            newSeqLineCand.SetStartTimeInAcq( oldSeqLineCand.GetStartTimeInAcq() );
            newSeqLineCand.SetStartFrequency( oldSeqLineCand.GetStartFrequency() );
            newSeqLineCand.SetComponent( oldSeqLineCand.GetComponent() );
            newSeqLineCand.SetAcquisitionID( oldSeqLineCand.GetAcquisitionID() );
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
            //KTDEBUG( itclog, "Adding points from oldSeqLineCand to newSeqLineCand: "<<pointIt->fTimeInRunC<<" "<<pointIt->fFrequency<<" "<<pointIt->fAmplitude<<" "<<pointIt->fNeighborhoodAmplitude );
            newSeqLineCand.AddPoint(*pointIt);
        }
    }

    bool KTIterativeTrackClustering::DoTheyMatch(KTProcessedTrackData& track1, KTProcessedTrackData& track2)
    {
        bool slopeCondition1 = std::abs(track1.GetEndFrequency()+track1.GetSlope()*(track2.GetStartTimeInRunC()-track1.GetEndTimeInRunC()) - track2.GetStartFrequency())<fFrequencyAcceptance;
        bool slopeCondition2 = std::abs(track2.GetStartFrequency()-track2.GetSlope()*(track2.GetStartTimeInRunC()-track1.GetEndTimeInRunC()) - track1.GetEndFrequency())<fFrequencyAcceptance;
        bool timeGapInLine = track1.GetEndTimeInRunC() <= track2.GetStartTimeInRunC();
        bool gapSmallerThanLimit = std::abs(track2.GetStartTimeInRunC() - track1.GetEndTimeInRunC())<fTimeGapTolerance;

        if (timeGapInLine and gapSmallerThanLimit and (slopeCondition1 or slopeCondition2))
        {
            return true;
        }

        slopeCondition1 = std::abs(track2.GetEndFrequency()+track2.GetSlope()*(track1.GetStartTimeInRunC()-track2.GetEndTimeInRunC()) - track1.GetStartFrequency())<fFrequencyAcceptance;
        slopeCondition2 = std::abs(track1.GetStartFrequency()-track1.GetSlope()*(track1.GetStartTimeInRunC()-track2.GetEndTimeInRunC()) - track2.GetEndFrequency())<fFrequencyAcceptance;
        timeGapInLine = track2.GetEndTimeInRunC() <= track1.GetStartTimeInRunC();
        gapSmallerThanLimit = std::abs(track1.GetStartTimeInRunC() - track2.GetEndTimeInRunC())<fTimeGapTolerance;

        if (timeGapInLine and gapSmallerThanLimit and (slopeCondition1 or slopeCondition2))
        {
            return true;
        }
    return false;
    }
    bool KTIterativeTrackClustering::DoTheyMatch(KTSequentialLineData& track1, KTSequentialLineData& track2)
    {
        bool slopeCondition1 = std::abs(track1.GetEndFrequency()+track1.GetSlope()*(track2.GetStartTimeInRunC()-track1.GetEndTimeInRunC()) - track2.GetStartFrequency())<fFrequencyAcceptance;
        bool slopeCondition2 = std::abs(track2.GetStartFrequency()-track2.GetSlope()*(track2.GetStartTimeInRunC()-track1.GetEndTimeInRunC()) - track1.GetEndFrequency())<fFrequencyAcceptance;
        bool timeGapInLine = track1.GetEndTimeInRunC() <= track2.GetStartTimeInRunC();
        bool gapSmallerThanLimit = std::abs(track2.GetStartTimeInRunC() - track1.GetEndTimeInRunC())<fTimeGapTolerance;

        if (timeGapInLine and gapSmallerThanLimit and (slopeCondition1 or slopeCondition2))
        {
            return true;
        }

        slopeCondition1 = std::abs(track2.GetEndFrequency()+track2.GetSlope()*(track1.GetStartTimeInRunC()-track2.GetEndTimeInRunC()) - track1.GetStartFrequency())<fFrequencyAcceptance;
        slopeCondition2 = std::abs(track1.GetStartFrequency()-track1.GetSlope()*(track1.GetStartTimeInRunC()-track2.GetEndTimeInRunC()) - track2.GetEndFrequency())<fFrequencyAcceptance;
        timeGapInLine = track2.GetEndTimeInRunC() <= track1.GetStartTimeInRunC();
        gapSmallerThanLimit = std::abs(track1.GetStartTimeInRunC() - track2.GetEndTimeInRunC())<fTimeGapTolerance;

        if (timeGapInLine and gapSmallerThanLimit and (slopeCondition1 or slopeCondition2))
        {
            return true;
        }
    return false;
    }

    bool KTIterativeTrackClustering::DoTheyOverlap(KTProcessedTrackData& track1, KTProcessedTrackData& track2)
    {
        // if the start time of track 2 is between start and end time of track 1
        bool condition1 = track2.GetStartTimeInRunC() < track1.GetEndTimeInRunC() and track2.GetStartTimeInRunC() >= track1.GetStartTimeInRunC();

        // and the start and end frequency of track 2 are close to track 1 (or an extrapolated track 1)
        bool condition2 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;

        // and the other end is nearby too
        bool condition3 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth * 5.0;
        // This condition doesn't need to be as strict and just makes sure this isn't a new track after all (instead one could compare slopes)

        if (condition1 and condition2 and condition3)
        {
            return true;
        }

        // the other way around
        bool condition4 = track1.GetStartTimeInRunC() < track2.GetEndTimeInRunC() and track1.GetStartTimeInRunC() >= track2.GetStartTimeInRunC();
        bool condition5 = std::abs(track1.GetStartFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth;
        bool condition6 = std::abs(track1.GetEndFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth * 5.0;

        if (condition4 and condition5 and condition6)
        {
            return true;
        }

        // same for endpoints overlapping in time
        condition1 = track2.GetEndTimeInRunC() <= track1.GetEndTimeInRunC() and track2.GetEndTimeInRunC() > track1.GetStartTimeInRunC();
        condition2 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;
        condition3 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth * 5.0;

        if (condition1 and condition2 and condition2)
        {
            return true;
        }

        // again the other way around
        condition4 = track1.GetEndTimeInRunC() <= track2.GetEndTimeInRunC() and track1.GetEndTimeInRunC() > track2.GetStartTimeInRunC();
        condition5 = std::abs(track1.GetEndFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth;
        condition6 = std::abs(track1.GetStartFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth * 5.0;

        if (condition4 and condition5 and condition6)
        {
            return true;
        }
        return false;
    }
    bool KTIterativeTrackClustering::DoTheyOverlap(KTSequentialLineData& track1, KTSequentialLineData& track2)
    {
        // if the start time of track 2 is between start and end time of track 1
        bool condition1 = track2.GetStartTimeInRunC() < track1.GetEndTimeInRunC() and track2.GetStartTimeInRunC() >= track1.GetStartTimeInRunC();

        // and the start and end frequency of track 2 are close to track 1 (or an extrapolated track 1)
        bool condition2 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;

        // and the other end is nearby too
        bool condition3 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth * 5.0;
        // This condition doesn't need to be as strict and just makes sure this isn't a new track after all (instead one could compare slopes)

        if (condition1 and condition2 and condition3)
        {
            return true;
        }

        // the other way around
        bool condition4 = track1.GetStartTimeInRunC() < track2.GetEndTimeInRunC() and track1.GetStartTimeInRunC() >= track2.GetStartTimeInRunC();
        bool condition5 = std::abs(track1.GetStartFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth;
        bool condition6 = std::abs(track1.GetEndFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth * 5.0;

        if (condition4 and condition5 and condition6)
        {
            return true;
        }

        // same for endpoints overlapping in time
        condition1 = track2.GetEndTimeInRunC() <= track1.GetEndTimeInRunC() and track2.GetEndTimeInRunC() > track1.GetStartTimeInRunC();
        condition2 = std::abs(track2.GetEndFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetEndTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth;
        condition3 = std::abs(track2.GetStartFrequency() - (track1.GetStartFrequency() + track1.GetSlope() * (track2.GetStartTimeInRunC() - track1.GetStartTimeInRunC()))) < fMaxTrackWidth * 5.0;

        if (condition1 and condition2 and condition2)
        {
            return true;
        }

        // again the other way around
        condition4 = track1.GetEndTimeInRunC() <= track2.GetEndTimeInRunC() and track1.GetEndTimeInRunC() > track2.GetStartTimeInRunC();
        condition5 = std::abs(track1.GetEndFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetEndTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth;
        condition6 = std::abs(track1.GetStartFrequency() - (track2.GetStartFrequency() + track2.GetSlope() * (track1.GetStartTimeInRunC() - track2.GetStartTimeInRunC()))) < fMaxTrackWidth * 5.0;

        if (condition4 and condition5 and condition6)
        {
            return true;
        }
        return false;
    }

    void KTIterativeTrackClustering::EmitTrackCandidates()
    {
        KTDEBUG(itclog, "Number of tracks to emit: "<<fCompTracks.size());
        KTINFO(itclog, "Clustering done.");

        std::vector<KTProcessedTrackData>::iterator trackIt = fCompTracks.begin();

        while(trackIt!=fCompTracks.end())
        {
            bool emitThisCandidate = true;

            if (fApplyTotalPowerCut)
            {
                if (trackIt->GetTotalPower() <= fTotalPowerThreshold)
                {
                    KTDEBUG(itclog, "track power below threshold: "<<trackIt->GetTotalPower()<<" "<<fTotalPowerThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAveragePowerCut)
            {
                if (trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <= fAveragePowerThreshold)
                {
                    KTDEBUG(itclog, "average track power below threshold: "<<trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <<" "<< fAveragePowerThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyTotalSNRCut)
            {
                if (trackIt->GetTotalPower() <= fTotalSNRThreshold)
                {
                    KTDEBUG(itclog, "total track snr below threshold: "<<trackIt->GetTotalPower()<<" "<<fTotalSNRThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAverageSNRCut)
            {
                if (trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <= fAverageSNRThreshold)
                {
                    KTDEBUG(itclog, "average track snr below threshold: "<<trackIt->GetTotalPower()<<" "<<fAverageSNRThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyTotalUnitlessResidualCut)
            {
                if (trackIt->GetTotalPower() <= fTotalUnitlessResidualThreshold)
                {
                    KTDEBUG(itclog, "total track residuals below threshold: "<<trackIt->GetTotalPower()<<" "<<fTotalUnitlessResidualThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAverageUnitlessResidualCut)
            {
                if (trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <= fAverageUnitlessResidualThreshold)
                {
                    KTDEBUG(itclog, "average track residuals below threshold: "<<trackIt->GetTotalPower()<<" "<<fAverageUnitlessResidualThreshold);
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

                KTINFO(itclog, "Now processing tracksCandidates");
                ProcessNewTrack( newTrack );

                KTDEBUG(itclog, "Emitting track signal");
                fCandidates.insert( data );
                fTrackSignal( data );
            }
            trackIt = fCompTracks.erase(trackIt);
        }
    }
    void KTIterativeTrackClustering::EmitSeqLineCandidates()
    {
        KTDEBUG(itclog, "Number of tracks to emit: "<<fCompSeqLineCands.size());
        KTINFO(itclog, "Clustering done.");


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
                    KTDEBUG(itclog, "track power below threshold: "<<summedPower<<" "<<fTotalPowerThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAveragePowerCut)
            {
                if (summedPower/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC()) <= fAveragePowerThreshold)
                {
                    KTDEBUG(itclog, "average track power below threshold: "<<summedPower/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC()) <<" "<< fAveragePowerThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyTotalSNRCut)
            {
                if (summedSNR <= fTotalSNRThreshold)
                {
                    KTDEBUG(itclog, "total track snr below threshold: "<<summedSNR<<" "<<fTotalSNRThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAverageSNRCut)
            {
                if (summedSNR/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC()) <= fAverageSNRThreshold)
                {
                    KTDEBUG(itclog, "average track snr below threshold: "<<summedSNR/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC())<<" "<<fAverageSNRThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyTotalUnitlessResidualCut)
            {
                if (summedUnitlessResidual <= fTotalUnitlessResidualThreshold)
                {
                    KTDEBUG(itclog, "total track residuals below threshold: "<<summedUnitlessResidual<<" "<<fTotalUnitlessResidualThreshold);
                    emitThisCandidate = false;
                }
            }
            if (fApplyAverageUnitlessResidualCut)
            {
                if (summedUnitlessResidual/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC()) <= fAverageUnitlessResidualThreshold)
                {
                    KTDEBUG(itclog, "average track residuals below threshold: "<<summedUnitlessResidual/(candIt->GetEndTimeInRunC()-candIt->GetStartTimeInRunC())<<" "<<fAverageUnitlessResidualThreshold);
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
                    //KTDEBUG( itclog, "Adding points to newSeqLineCand: "<<pointIt->fTimeInRunC<<" "<<pointIt->fFrequency<<" "<<pointIt->fAmplitude<<" "<<pointIt->fNeighborhoodAmplitude );
                    newSeqLineCand.AddPoint( *pointIt );
                }
                KTDEBUG( itclog, "Emitting SeqLine signal" );
                fCandidates.insert( data );
                fSeqLineCandSignal( data );
            }
        candIt = fCompSeqLineCands.erase(candIt);
        }
    }
    const void KTIterativeTrackClustering::ProcessNewTrack( KTProcessedTrackData& myNewTrack )
    {
        myNewTrack.SetTimeLength( myNewTrack.GetEndTimeInRunC() - myNewTrack.GetStartTimeInRunC() );
        myNewTrack.SetFrequencyWidth( myNewTrack.GetEndFrequency() - myNewTrack.GetStartFrequency() );
        myNewTrack.SetSlope( myNewTrack.GetFrequencyWidth() / myNewTrack.GetTimeLength() );
        myNewTrack.SetIntercept( myNewTrack.GetStartFrequency() - myNewTrack.GetSlope() * myNewTrack.GetStartTimeInRunC() );
    }


} /* namespace Katydid */
