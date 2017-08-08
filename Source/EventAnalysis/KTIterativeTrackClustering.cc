/*
 * KTCollinearTrackClustering.cc
 *
 *  Created on: August 7, 2017
 *      Author: C. Claessens
 */

#include "KTIterativeTrackClustering.hh"

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
    KTLOGGER(itclog, "KTIterativeTrackClustering");

    KT_REGISTER_PROCESSOR(KTIterativeTrackClustering, "iterative-track-clustering");

    KTIterativeTrackClustering::KTIterativeTrackClustering(const std::string& name) :
            KTPrimaryProcessor(name),
            fTimeGapTolerance(5.*pow(10, -3)),
            fFrequencyAcceptance(185.*pow(10, 5)),
            fCompTracks(),
            fNewTracks(),
            fNTracks(0),
            fApplyPowerCut(true),
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
            SetFrequencyAcceptance(node->get_value<int>("frequency-acceptance"));
        }
        if (node->has("apply-power-cut"))
        {
            SetApplyPowerCut(node->get_value<bool>("apply-power-cut"));
        }
        if (node->has("apply-point-density-cut"))
        {
            SetApplyDensityCut(node->get_value<bool>("apply-point-density-cut"));
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

        // verify that we have the right number of components
        //if (track.GetComponent() >= fCompTracks.size())
        //{
        //    SetNComponents(track.GetComponent() + 1);
        //}

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

        fNewTracks.clear();

        unsigned NumberOfTracks = fCompTracks.size();
        unsigned NumberOfNewTracks = fNewTracks.size();


        //std::vector<newTrack> newTracks;

        // loop over components
        //unsigned component = 0;
        unsigned loopCounter = 0;

        if (NumberOfTracks > 1)
        {
            while (NumberOfTracks!=NumberOfNewTracks and loopCounter < 5)
            {
                NumberOfTracks = fCompTracks.size();
                KTDEBUG(itclog, "Number of tracks to cluster: "<< NumberOfTracks);
                loopCounter ++;
                this->LineClustering();

                // Update number of tracks
                NumberOfNewTracks = fNewTracks.size();

                KTDEBUG(itclog, "Number of new tracks: "<< NumberOfNewTracks);

                fCompTracks.clear();
                fCompTracks = fNewTracks;
                fNewTracks.clear();
            }
        }
        this->EmitTrackCandidates();

        return true;
    }

    bool KTIterativeTrackClustering::LineClustering()
    {
        bool match = false;
        for (std::vector<KTProcessedTrackData>::iterator compIt = fCompTracks.begin(); compIt != fCompTracks.end(); ++compIt)
        {
            KTINFO(itclog,"track power: "<<compIt->GetTotalPower());
            for (std::vector<KTProcessedTrackData>::iterator newIt = fNewTracks.begin(); newIt != fNewTracks.end(); ++newIt)
            {
                if (this->DoTheyMatch(*compIt, *newIt))
                {
                    match = true;
                    KTDEBUG(itclog, "Found matching tracks");
                    this->CombineTracks(*compIt, *newIt);
                }
                else if (this->DoTheyOverlap(*compIt, *newIt))
                {
                    KTDEBUG(itclog, "Found overlapping tracks")
                    match = true;
                    this->CombineTracks(*compIt, *newIt);
                }
            }
            if (match == false)
            {
                KTProcessedTrackData newTrack(*compIt);
                KTDEBUG(itclog, "Creating new track: "<< newTrack.GetTotalPower());

                fNewTracks.push_back(newTrack);
            }
        }
        return true;
    }

    void KTIterativeTrackClustering::CombineTracks(KTProcessedTrackData& oldTrack, KTProcessedTrackData& newTrack)
    {
        if (oldTrack.GetStartTimeInRunC() < newTrack.GetStartTimeInRunC())
        {
            newTrack.SetStartTimeInRunC( oldTrack.GetStartTimeInRunC());
            newTrack.SetStartTimeInAcq( oldTrack.GetStartTimeInAcq());
            newTrack.SetStartFrequency( oldTrack.GetStartFrequency());
            newTrack.SetIntercept( oldTrack.GetIntercept());
            newTrack.SetInterceptSigma( oldTrack.GetInterceptSigma());
            newTrack.SetStartTimeInRunCSigma( oldTrack.GetStartTimeInRunCSigma());
            newTrack.SetStartFrequencySigma( oldTrack.GetStartFrequencySigma());
        }
        if (oldTrack.GetEndTimeInRunC() > oldTrack.GetEndTimeInRunC())
        {
            newTrack.SetEndTimeInRunC( oldTrack.GetEndTimeInRunC());
            newTrack.SetEndFrequency( oldTrack.GetEndFrequency());
            newTrack.SetEndTimeInRunCSigma( oldTrack.GetEndTimeInRunCSigma());
            newTrack.SetEndFrequencySigma( oldTrack.GetEndFrequencySigma());

        }
        newTrack.SetSlope( (newTrack.GetEndFrequency() - newTrack.GetStartFrequency())/(newTrack.GetEndTimeInRunC() - newTrack.GetStartTimeInRunC()));
        newTrack.SetTotalPower( newTrack.GetTotalPower() + oldTrack.GetTotalPower());
        newTrack.SetTimeLength( newTrack.GetEndTimeInRunC() - newTrack.GetStartTimeInRunC());

        newTrack.SetSlopeSigma( sqrt(newTrack.GetSlopeSigma()*newTrack.GetSlopeSigma() + oldTrack.GetSlopeSigma()*oldTrack.GetSlopeSigma()));
        KTDEBUG(itclog, "Combining tracks, power before and after: "<<oldTrack.GetTotalPower()<<" "<<newTrack.GetTotalPower());
    }


    bool KTIterativeTrackClustering::DoTheyMatch(KTProcessedTrackData& Track1, KTProcessedTrackData& Track2)
    {
        bool slope_condition1 = std::abs(Track1.GetEndFrequency()+Track1.GetSlope()*(Track2.GetStartTimeInRunC()-Track1.GetEndTimeInRunC()) - Track2.GetStartFrequency())<fFrequencyAcceptance;
        bool slope_condition2 = std::abs(Track2.GetStartFrequency()-Track2.GetSlope()*(Track2.GetStartTimeInRunC()-Track1.GetEndTimeInRunC()) - Track1.GetEndFrequency())<fFrequencyAcceptance;
        bool time_gap_in_line = Track1.GetEndTimeInRunC() < Track2.GetStartTimeInRunC();
        bool gap_smaller_than_limit = std::abs(Track2.GetStartTimeInRunC() - Track1.GetEndTimeInRunC())<fTimeGapTolerance;

        if (time_gap_in_line and gap_smaller_than_limit and (slope_condition1 or slope_condition2))
        {
            return true;
        }

        slope_condition1 = std::abs(Track2.GetEndFrequency()+Track2.GetSlope()*(Track1.GetStartTimeInRunC()-Track2.GetEndTimeInRunC()) - Track1.GetStartFrequency())<fFrequencyAcceptance;
        slope_condition2 = std::abs(Track1.GetStartFrequency()-Track1.GetSlope()*(Track1.GetStartTimeInRunC()-Track2.GetEndTimeInRunC()) - Track2.GetEndFrequency())<fFrequencyAcceptance;
        time_gap_in_line = Track2.GetEndTimeInRunC() < Track1.GetStartTimeInRunC();
        gap_smaller_than_limit = std::abs(Track1.GetStartTimeInRunC() - Track2.GetEndTimeInRunC())<fTimeGapTolerance;

        if (time_gap_in_line and gap_smaller_than_limit and (slope_condition1 or slope_condition2))
        {
            return true;
        }

    return false;
    }

    bool KTIterativeTrackClustering::DoTheyOverlap(KTProcessedTrackData& Track1, KTProcessedTrackData& Track2)
    {
        // if the start time of track 2 is between start and end time of track 1
        bool condition1 = Track2.GetStartTimeInRunC() < Track1.GetEndTimeInRunC() and Track2.GetStartTimeInRunC() > Track1.GetStartTimeInRunC();

        // and the start frequency of track 2 is too close to track 1
        bool condition2 = std::abs(Track2.GetStartFrequency() - (Track1.GetStartFrequency() + Track1.GetSlope() * (Track2.GetStartTimeInRunC() - Track1.GetStartTimeInRunC()))) < fFrequencyAcceptance;

        if (condition1 or condition2)
        {
            return true;
        }
        // the other way around
        bool condition3 = Track1.GetStartTimeInRunC() < Track2.GetEndTimeInRunC() and Track1.GetStartTimeInRunC() > Track2.GetStartTimeInRunC();
        bool condition4 = std::abs(Track1.GetStartFrequency() - (Track2.GetStartFrequency() + Track2.GetSlope() * (Track1.GetStartTimeInRunC() - Track2.GetStartTimeInRunC()))) < fFrequencyAcceptance;

        if (condition3 or condition4)
        {
            return true;
        }

        // same for end point of track2
        condition1 = Track2.GetEndTimeInRunC() < Track1.GetEndTimeInRunC() and Track2.GetEndTimeInRunC() > Track1.GetStartTimeInRunC();

        // and the start frequency of track 2 is too close to track 1
        condition2 = std::abs(Track2.GetEndFrequency() - (Track1.GetStartFrequency() + Track1.GetSlope() * (Track2.GetEndTimeInRunC() - Track1.GetStartTimeInRunC()))) < fFrequencyAcceptance;

        if (condition1 or condition2)
        {
            return true;
        }
        // the other way around
        condition3 = Track1.GetEndTimeInRunC() < Track2.GetEndTimeInRunC() and Track1.GetEndTimeInRunC() > Track2.GetStartTimeInRunC();
        condition4 = std::abs(Track1.GetEndFrequency() - (Track2.GetStartFrequency() + Track2.GetSlope() * (Track1.GetEndTimeInRunC() - Track2.GetStartTimeInRunC()))) < fFrequencyAcceptance;

        if (condition3 or condition4)
        {
            return true;
        }

/*
        // if track2 is within same time range as track1 and they cross one way
        if (Track1.fStartTimeInRunC < Track2.fStartTimeInRunC and Track1.fEndTimeInRunC > Track2.fEndTimeInRunC and Track1.fStartFrequency < Track2.fStartFrequency and Track1.fEndFrequency > Track2.fEndFrequency)
        {
            return true;
        }
        // or the other
        if (Track1.fStartTimeInRunC < Track2.fStartTimeInRunC and Track1.fEndTimeInRunC > Track2.fEndTimeInRunC and Track1.fStartFrequency > Track2.fStartFrequency and Track1.fEndFrequency < Track2.fEndFrequency)
        {
            return true;
        }
        // and the other way around
        if (Track2.fStartTimeInRunC < Track1.fStartTimeInRunC and Track2.fEndTimeInRunC > Track1.fEndTimeInRunC and Track2.fStartFrequency < Track1.fStartFrequency and Track2.fEndFrequency > Track1.fEndFrequency)
        {
            return true;
        }
        if (Track2.fStartTimeInRunC < Track1.fStartTimeInRunC and Track2.fEndTimeInRunC > Track1.fEndTimeInRunC and Track2.fStartFrequency > Track1.fStartFrequency and Track2.fEndFrequency < Track1.fEndFrequency)
        {
            return true;
        }*/
        return false;
    }


    /*void KTIterativeTrackClustering::EmitPreCandidatesAsCandidates()
    {
        KTINFO(itclog, "No new tracks found. Emitting old tracks as new tracks");
        for (std::vector<KTProcessedTrackData>::const_iterator trackIt = fCompTracks.begin(); trackIt!=fCompTracks.end(); trackIt++)
        {
            fTrackSignal(*trackIt);
        }
    }
*/
    void KTIterativeTrackClustering::EmitTrackCandidates()
    {
        KTDEBUG(itclog, "Number of tracks to emit: "<<fCompTracks.size());
        bool LineIsTrack;
        KTINFO(itclog, "Clustering done. Emitting new tracks followed by clustering-done signal")

        std::vector<KTProcessedTrackData>::iterator trackIt = fCompTracks.begin();

        while(trackIt!=fCompTracks.end())
        {
            LineIsTrack = true;

            if (fApplyPowerCut)
            {
                if (trackIt->GetTotalPower() <= fPowerThreshold)
                {
                    KTDEBUG(itclog, "track power bellow threshold: "<<trackIt->GetTotalPower()<<" "<<fPowerThreshold);
                    LineIsTrack = false;
                }
            }
            if (fApplyDensityCut)
            {
                if (trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <= fDensityThreshold)
                {
                    KTDEBUG(itclog, "track power density bellow threshold: "<<trackIt->GetTotalPower()/(trackIt->GetEndTimeInRunC()-trackIt->GetStartTimeInRunC()) <<" "<< fDensityThreshold);
                    LineIsTrack = false;
                }
            }

            if (LineIsTrack == true)
            {
                // Set up new data object
                Nymph::KTDataPtr data( new Nymph::KTData() );
                KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
                newTrack.SetComponent( trackIt->GetComponent() );
                newTrack.SetTrackID(fNTracks);
                fNTracks++;

                newTrack.SetStartTimeInRunC( trackIt->GetStartTimeInRunC());
                newTrack.SetEndTimeInRunC( trackIt->GetEndTimeInRunC());
                newTrack.SetStartTimeInAcq( trackIt->GetStartTimeInAcq());
                newTrack.SetStartFrequency( trackIt->GetStartFrequency());
                newTrack.SetEndFrequency( trackIt->GetEndFrequency());
                newTrack.SetSlope(trackIt->GetSlope());


                // Process & emit new track

                KTINFO(itclog, "Now processing tracksCandidates");
                ProcessNewTrack( newTrack );

                KTDEBUG(itclog, "Emitting track signal");
                fTrackSignal( data );
            }
            trackIt = fCompTracks.erase(trackIt);
        }
    }

    void KTIterativeTrackClustering::ProcessNewTrack( KTProcessedTrackData& myNewTrack )
    {
        myNewTrack.SetTimeLength( myNewTrack.GetEndTimeInRunC() - myNewTrack.GetStartTimeInRunC() );
        myNewTrack.SetFrequencyWidth( myNewTrack.GetEndFrequency() - myNewTrack.GetStartFrequency() );
        myNewTrack.SetSlope( myNewTrack.GetFrequencyWidth() / myNewTrack.GetTimeLength() );
        myNewTrack.SetIntercept( myNewTrack.GetStartFrequency() - myNewTrack.GetSlope() * myNewTrack.GetStartTimeInRunC() );
    }


} /* namespace Katydid */
