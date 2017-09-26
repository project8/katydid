/*
 * KTIterativeTrackClustering.cc
 *
 *  Created on: August 7, 2017
 *      Author: C. Claessens
 */

#include "KTIterativeTrackClustering.hh"
#include "KTLogger.hh"

#include <vector>

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
            fNTracks(0),
            fApplyPowerCut(false),
            fApplyDensityCut(false),
            fPowerThreshold(0.0),
            fDensityThreshold(0.0),
            fTrackSignal("track", this),
            fDoneSignal("tracks-done", this),
            fTakeTrackSlot("track", this, &KTIterativeTrackClustering::TakeTrack)
    {
        RegisterSlot("do-clustering", this, &KTIterativeTrackClustering::DoClusteringSlot);
    }

    KTIterativeTrackClustering::~KTIterativeTrackClustering()
    {
    }

    bool KTIterativeTrackClustering::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (node->has("time-gap-tolerance"))
        {
            SetTimeGapTolerance(node->get_value<double>("time-gap-tolerance"));
        }
        if (node->has("frequency-acceptance"))
        {
            SetFrequencyAcceptance(node->get_value<double>("frequency-acceptance"));
        }
        if (node->has("max-track-width"))
        {
            SetMaxTrackWidth(node->get_value<double>("max-track-width"));
        }
        if (node->has("apply-power-cut"))
        {
            SetApplyPowerCut(node->get_value<bool>("apply-power-cut"));
        }
        if (node->has("apply-power-density-cut"))
        {
            SetApplyDensityCut(node->get_value<bool>("apply-power-density-cut"));
        }
        if (node->has("power-threshold"))
        {
            SetPowerThreshold(node->get_value<double>("power-threshold"));
        }
        if (node->has("power-density-threshold"))
        {
            SetDensityThreshold(node->get_value<double>("power-density-threshold"));
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
        return DoClustering();
    }

    bool KTIterativeTrackClustering::DoClustering()
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
                this->ExtrapolateClustering();

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


    bool KTIterativeTrackClustering::ExtrapolateClustering()
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
        newTrack.SetTimeLength( newTrack.GetEndTimeInRunC() - newTrack.GetStartTimeInRunC());
        newTrack.SetSlopeSigma( sqrt(newTrack.GetSlopeSigma()*newTrack.GetSlopeSigma() + oldTrack.GetSlopeSigma()*oldTrack.GetSlopeSigma()));
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


    void KTIterativeTrackClustering::EmitTrackCandidates()
    {
        KTDEBUG(itclog, "Number of tracks to emit: "<<fCompTracks.size());
        bool lineIsTrack true;
        KTINFO(itclog, "Clustering done.");

        std::vector<KTProcessedTrackData>::iterator trackIt = fCompTracks.begin();

        while(trackIt!=fCompTracks.end())
        {
            lineIsTrack = true;

            if (fApplyPowerCut)
            {
                if (trackIt->GetTotalPower() <= fPowerThreshold)
                {
                    KTDEBUG(itclog, "track power below threshold: "<<trackIt->GetTotalPower()<<" "<<fPowerThreshold);
                    lineIsTrack = false;
                }
            }
            if (fApplyDensityCut)
            {
                if (trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <= fDensityThreshold)
                {
                    KTDEBUG(itclog, "track power density below threshold: "<<trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <<" "<< fDensityThreshold);
                    lineIsTrack = false;
                }
            }

            if (lineIsTrack == true)
            {
                // Set up new data object
                Nymph::KTDataPtr data( new Nymph::KTData() );
                KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
                newTrack.SetComponent( trackIt->GetComponent() );
                newTrack.SetAcquisitionID( trackIt->GetAcquisitionID());
                newTrack.SetTrackID(fNTracks);
                fNTracks++;

                newTrack.SetStartTimeInRunC( trackIt->GetStartTimeInRunC());
                newTrack.SetEndTimeInRunC( trackIt->GetEndTimeInRunC());
                newTrack.SetStartTimeInAcq( trackIt->GetStartTimeInAcq());
                newTrack.SetStartFrequency( trackIt->GetStartFrequency());
                newTrack.SetEndFrequency( trackIt->GetEndFrequency());
                newTrack.SetSlope(trackIt->GetSlope());
                newTrack.SetTotalPower(trackIt->GetTotalPower());


                // Process & emit new track

                KTINFO(itclog, "Now processing tracksCandidates");
                ProcessNewTrack( newTrack );

                KTDEBUG(itclog, "Emitting track signal");
                fTrackSignal( data );
            }
            trackIt = fCompTracks.erase(trackIt);
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
