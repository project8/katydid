/*
 * KTOverlappingTrackClustering.cc
 *
 *  Created on: August 7, 2017
 *      Author: C. Claessens
 */

#include "KTOverlappingTrackClustering.hh"

#include "KTLogger.hh"

#include <vector>

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
            fMaxTrackWidth(185000.),
            fCompTracks(),
            fNewTracks(),
            fNTracks(0),
            fApplyPowerCut(false),
            fApplyDensityCut(false),
            fPowerThreshold(0.0),
            fDensityThreshold(0.0),
            fTrackSignal("track", this),
            fDoneSignal("tracks-done", this),
            fTakeTrackSlot("track", this, &KTOverlappingTrackClustering::TakeTrack)
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

    bool KTOverlappingTrackClustering::TakeTrack(KTProcessedTrackData& track)
    {
        // ignore the track if it's been cut
        if (track.GetIsCut()) return true;


        KTDEBUG(otclog, "Taking track: (" << track.GetStartTimeInRunC() << ", " << track.GetStartFrequency() << ", " << track.GetEndTimeInRunC() << ", " << track.GetEndFrequency() << ")");

        // copy the full track data
        fCompTracks.push_back(track);

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
        return DoClustering();
    }

    bool KTOverlappingTrackClustering::DoClustering()
    {
        if (! FindMatchingTracks())
        {
            KTERROR(otclog, "An error occurred while identifying overlapping tracks");
            return false;
        }

        KTDEBUG(otclog, "Track building complete");
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
        //newTrack.SetSlope( (newTrack.GetEndFrequency() - newTrack.GetStartFrequency())/(newTrack.GetEndTimeInRunC() - newTrack.GetStartTimeInRunC()));
        newTrack.SetTotalPower( newTrack.GetTotalPower() + oldTrack.GetTotalPower());
        newTrack.SetTimeLength( newTrack.GetEndTimeInRunC() - newTrack.GetStartTimeInRunC());

        newTrack.SetSlopeSigma( sqrt(newTrack.GetSlopeSigma()*newTrack.GetSlopeSigma() + oldTrack.GetSlopeSigma()*oldTrack.GetSlopeSigma()));
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
        bool lineIsTrack = true;
        KTINFO(otclog, "Clustering done.");

        std::vector<KTProcessedTrackData>::iterator trackIt = fCompTracks.begin();

        while(trackIt!=fCompTracks.end())
        {
            lineIsTrack = true;

            if (fApplyPowerCut)
            {
                if (trackIt->GetTotalPower() <= fPowerThreshold)
                {
                    KTDEBUG(otclog, "Track total power below threshold: "<<trackIt->GetTotalPower()<<" "<<fPowerThreshold);
                    lineIsTrack = false;
                }
            }
            if (fApplyDensityCut)
            {
                if (trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <= fDensityThreshold)
                {
                    KTDEBUG(otclog, "Track power density below threshold: "<<trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <<" "<< fDensityThreshold);
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

                KTINFO(otclog, "Now processing tracksCandidates");
                ProcessNewTrack( newTrack );

                KTDEBUG(otclog, "Emitting track signal");
                fTrackSignal( data );
            }
            trackIt = fCompTracks.erase(trackIt);
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
