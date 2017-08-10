/*
 * KTOverlappingTrackClustering.cc
 *
 *  Created on: August 7, 2017
 *      Author: C. Claessens
 */

#include "KTOverlappingTrackClustering.hh"

#include "KTLogger.hh"

#include <list>
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
            fMaxTrackWidth(185.*pow(10, 3)),
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

        unsigned NumberOfTracks = fCompTracks.size();
        unsigned NumberOfNewTracks = fNewTracks.size();



        if (NumberOfTracks > 1)
        {
            while (NumberOfTracks!=NumberOfNewTracks)
            {
                NumberOfTracks = fCompTracks.size();
                KTDEBUG(otclog, "Number of tracks to cluster: "<< NumberOfTracks);
                this->OverlapClustering();

                // Update number of tracks
                NumberOfNewTracks = fNewTracks.size();

                KTDEBUG(otclog, "Number of new tracks: "<< NumberOfNewTracks);

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
        bool match;
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
            
                if (this->DoTheyCross(*compIt, *newIt))
                {
                    match = true;
                    KTDEBUG(otclog, "Found crossing tracks")
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


    void KTOverlappingTrackClustering::CombineTracks(KTProcessedTrackData& oldTrack, KTProcessedTrackData& newTrack)
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

/*
    bool KTOverlappingTrackClustering::DoTheyMatch(KTProcessedTrackData& Track1, KTProcessedTrackData& Track2)
    {
        bool slope_condition1 = std::abs(Track1.GetEndFrequency()+Track1.GetSlope()*(Track2.GetStartTimeInRunC()-Track1.GetEndTimeInRunC()) - Track2.GetStartFrequency())<fMaxTrackWidth;
        bool slope_condition2 = std::abs(Track2.GetStartFrequency()-Track2.GetSlope()*(Track2.GetStartTimeInRunC()-Track1.GetEndTimeInRunC()) - Track1.GetEndFrequency())<fMaxTrackWidth;
        bool time_gap_in_line = Track1.GetEndTimeInRunC() < Track2.GetStartTimeInRunC();
        bool gap_smaller_than_limit = std::abs(Track2.GetStartTimeInRunC() - Track1.GetEndTimeInRunC())<fTimeGapTolerance;

        if (time_gap_in_line and gap_smaller_than_limit and (slope_condition1 or slope_condition2))
        {
            return true;
        }

        slope_condition1 = std::abs(Track2.GetEndFrequency()+Track2.GetSlope()*(Track1.GetStartTimeInRunC()-Track2.GetEndTimeInRunC()) - Track1.GetStartFrequency())<fMaxTrackWidth;
        slope_condition2 = std::abs(Track1.GetStartFrequency()-Track1.GetSlope()*(Track1.GetStartTimeInRunC()-Track2.GetEndTimeInRunC()) - Track2.GetEndFrequency())<fMaxTrackWidth;
        time_gap_in_line = Track2.GetEndTimeInRunC() < Track1.GetStartTimeInRunC();
        gap_smaller_than_limit = std::abs(Track1.GetStartTimeInRunC() - Track2.GetEndTimeInRunC())<fTimeGapTolerance;

        if (time_gap_in_line and gap_smaller_than_limit and (slope_condition1 or slope_condition2))
        {
            return true;
        }

    return false;
    }
*/
    bool KTOverlappingTrackClustering::DoTheyOverlap(KTProcessedTrackData& Track1, KTProcessedTrackData& Track2)
    {
        // if the start time of track 2 is between start and end time of track 1
        bool condition1 = Track2.GetStartTimeInRunC() < Track1.GetEndTimeInRunC() and Track2.GetStartTimeInRunC() >= Track1.GetStartTimeInRunC();

        // and the start frequency of track 2 is too close to track 1
        bool condition2 = std::abs(Track2.GetStartFrequency() - (Track1.GetStartFrequency() + Track1.GetSlope() * (Track2.GetStartTimeInRunC() - Track1.GetStartTimeInRunC()))) < fMaxTrackWidth;

        if (condition1 and condition2)
        {
            return true;
        }
        // the other way around
        bool condition3 = Track1.GetStartTimeInRunC() < Track2.GetEndTimeInRunC() and Track1.GetStartTimeInRunC() >= Track2.GetStartTimeInRunC();
        bool condition4 = std::abs(Track1.GetStartFrequency() - (Track2.GetStartFrequency() + Track2.GetSlope() * (Track1.GetStartTimeInRunC() - Track2.GetStartTimeInRunC()))) < fMaxTrackWidth;

        if (condition3 and condition4)
        {
            return true;
        }
        // same for end point of track2
        condition1 = Track2.GetEndTimeInRunC() <= Track1.GetEndTimeInRunC() and Track2.GetEndTimeInRunC() > Track1.GetStartTimeInRunC();
        condition2 = std::abs(Track2.GetEndFrequency() - (Track1.GetStartFrequency() + Track1.GetSlope() * (Track2.GetEndTimeInRunC() - Track1.GetStartTimeInRunC()))) < fMaxTrackWidth;

        if (condition1 and condition2)
        {
            return true;
        }
        // the other way around
        condition3 = Track1.GetEndTimeInRunC() <= Track2.GetEndTimeInRunC() and Track1.GetEndTimeInRunC() > Track2.GetStartTimeInRunC();
        condition4 = std::abs(Track1.GetEndFrequency() - (Track2.GetStartFrequency() + Track2.GetSlope() * (Track1.GetEndTimeInRunC() - Track2.GetStartTimeInRunC()))) < fMaxTrackWidth;

        if (condition3 and condition4)
        {
            return true;
        }

        return false;
    }

    bool KTOverlappingTrackClustering::DoTheyCross(KTProcessedTrackData& Track1, KTProcessedTrackData& Track2)
    {
        // if the start time or the end time of track 2 is between start and end time of track 1
        bool condition1 = Track2.GetStartTimeInRunC() < Track1.GetEndTimeInRunC() and Track2.GetStartTimeInRunC() > Track1.GetStartTimeInRunC();
        bool condition2 = Track2.GetEndTimeInRunC() < Track1.GetEndTimeInRunC() and Track2.GetEndTimeInRunC() > Track1.GetStartTimeInRunC();

        // and they cross in frequency this way
        bool cross_condition1 = Track2.GetStartFrequency() < (Track1.GetStartFrequency() + Track1.GetSlope() * (Track2.GetStartTimeInRunC() - Track1.GetStartTimeInRunC()));
        bool cross_condition2 = Track2.GetEndFrequency() > (Track1.GetStartFrequency() + Track1.GetSlope() * (Track2.GetEndTimeInRunC() - Track1.GetStartTimeInRunC()));

        // or the other way around
        bool cross_condition3 = Track2.GetStartFrequency() > (Track1.GetStartFrequency() + Track1.GetSlope() * (Track2.GetStartTimeInRunC() - Track1.GetStartTimeInRunC()));
        bool cross_condition4 = Track2.GetEndFrequency() < (Track1.GetStartFrequency() + Track1.GetSlope() * (Track2.GetEndTimeInRunC() - Track1.GetStartTimeInRunC()));


        if ((condition1 or condition2) and ((cross_condition1 and cross_condition2) or (cross_condition3 and cross_condition4)))
        {
            return true;
        }

        // the same as above but for the inverted track combination
        condition1 = Track1.GetStartTimeInRunC() < Track2.GetEndTimeInRunC() and Track1.GetStartTimeInRunC() > Track2.GetStartTimeInRunC();
        condition2 = Track1.GetEndTimeInRunC() < Track2.GetEndTimeInRunC() and Track1.GetEndTimeInRunC() > Track2.GetStartTimeInRunC();
   
        // and they cross in frequency this way
        cross_condition1 = Track1.GetStartFrequency() < (Track2.GetStartFrequency() + Track2.GetSlope() * (Track1.GetStartTimeInRunC() - Track2.GetStartTimeInRunC()));
        cross_condition2 = Track1.GetEndFrequency() > (Track2.GetStartFrequency() + Track2.GetSlope() * (Track1.GetEndTimeInRunC() - Track2.GetStartTimeInRunC()));
        
        // or the other way around
        cross_condition3 = Track1.GetStartFrequency() > (Track2.GetStartFrequency() + Track2.GetSlope() * (Track1.GetStartTimeInRunC() - Track2.GetStartTimeInRunC()));
        cross_condition4 = Track1.GetEndFrequency() < (Track2.GetStartFrequency() + Track2.GetSlope() * (Track1.GetEndTimeInRunC() - Track2.GetStartTimeInRunC()));
   
   
        if ((condition1 or condition2) and ((cross_condition1 and cross_condition2) or (cross_condition3 and cross_condition4)))
        {   
            return true;
        }


        return false;
    }

    void KTOverlappingTrackClustering::EmitTrackCandidates()
    {
        KTDEBUG(otclog, "Number of tracks to emit: "<<fCompTracks.size());
        bool LineIsTrack;
        KTINFO(otclog, "Clustering done.")

        std::vector<KTProcessedTrackData>::iterator trackIt = fCompTracks.begin();

        while(trackIt!=fCompTracks.end())
        {
            LineIsTrack = true;

            if (fApplyPowerCut)
            {
                if (trackIt->GetTotalPower() <= fPowerThreshold)
                {
                    KTDEBUG(otclog, "Track total power below threshold: "<<trackIt->GetTotalPower()<<" "<<fPowerThreshold);
                    LineIsTrack = false;
                }
            }
            if (fApplyDensityCut)
            {
                if (trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <= fDensityThreshold)
                {
                    KTDEBUG(otclog, "Track power density below threshold: "<<trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <<" "<< fDensityThreshold);
                    LineIsTrack = false;
                }
            }

            if (LineIsTrack == true)
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

    void KTOverlappingTrackClustering::ProcessNewTrack( KTProcessedTrackData& myNewTrack )
    {
        myNewTrack.SetTimeLength( myNewTrack.GetEndTimeInRunC() - myNewTrack.GetStartTimeInRunC() );
        myNewTrack.SetFrequencyWidth( myNewTrack.GetEndFrequency() - myNewTrack.GetStartFrequency() );
        myNewTrack.SetSlope( myNewTrack.GetFrequencyWidth() / myNewTrack.GetTimeLength() );
        myNewTrack.SetIntercept( myNewTrack.GetStartFrequency() - myNewTrack.GetSlope() * myNewTrack.GetStartTimeInRunC() );
    }


} /* namespace Katydid */
