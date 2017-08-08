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
            fTimeGapTolerance(185.*pow(10, 5)),
            fFrequencyAcceptance(5.*pow(10, -3)),
            fCompTracks(1),
            fNewTracks(1),
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
        if (track.GetComponent() >= fCompTracks.size())
        {
            SetNComponents(track.GetComponent() + 1);
        }

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

        KTDEBUG(itclog, "Number of tracks to cluster: "<< NumberOfTracks);
        KTDEBUG(itclog, "Number of new tracks: "<< NumberOfNewTracks);

        //std::vector<newTrack> newTracks;

        // loop over components
        unsigned component = 0;
        unsigned loopCounter = 0;

        if (NumberOfTracks > 1)
        {
            while (NumberOfTracks!=NumberOfNewTracks and loopCounter < 5)
            {
                KTDEBUG(itclog, "Number of tracks to cluster: "<< NumberOfTracks);
                KTDEBUG(itclog, "Number of new tracks: "<< NumberOfNewTracks);
                loopCounter ++;
                this->LineClustering();

                // Update number of tracks
                NumberOfTracks = fCompTracks.size();
                NumberOfNewTracks = fNewTracks.size();

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
                fNewTracks.push_back(newTrack);
            }
        }
        return true;
    }

    void KTIterativeTrackClustering::CombineTracks(KTProcessedTrackData& oldTrack, KTProcessedTrackData& newTrack)
    {
        if (oldTrack.fStartTimeInRunC < newTrack.fStartTimeInRunC)
        {
            newTrack.fStartTimeInRunC = oldTrack.fStartTimeInRunC;
            newTrack.fStartTimeInAcq = oldTrack.fStartTimeInAcq;
            newTrack.fStartFrequency = oldTrack.fStartFrequency;
            newTrack.fIntercept = oldTrack.fIntercept;
            newTrack.fInterceptSigma = oldTrack.fInterceptSigmaM;
            newTrack.fStartTimeInRunCSigma = oldTrack.fStartTimeInRunCSigma;
            newTrack.fStartFrequencySigma = oldTrack.fStartFrequencySigma;
        }
        if (oldTrack.fEndTimeInRunC > oldTrack.fEndTimeInRunC)
        {
            newTrack.fEndTimeInRunC = oldTrack.fEndTimeInRunC;
            newTrack.fEndFrequency = oldTrack.fEndFrequency;
            newTrack.fEndTimeInRunCSigma = oldTrack.fEndTimeInRunCSigma;
            newTrack.fEndFrequencySigma = oldTrack.fEndFrequenySigma;

        }
        newTrack.fSlope = (newTrack.fEndFrequency - newTrack.fStartFrequency)/(newTrack.fEndTimeInRunC - newTrack.fStartTimeInRunC);
        newTrack.fTotalPower = newTrack.fTotalPower + oldTrack.fTotalPower;
        newTrack.fTimeLength = newTrack.fEndTimeInRunC - newTrack.fStartTimeInRunC;

        newTrack.fSlopeSigma = sqrt(newTrack.fSlopeSigma*newTrack.fSlopeSigma + oldTrack.fSlopeSigma*oldTrack.fSlopeSigma);
    }


    bool KTIterativeTrackClustering::DoTheyMatch(KTProcessedTrackData& Track1, KTProcessedTrackData& Track2)
    {
        bool slope_condition1 = std::abs(Track1.fEndFrequency+Track1.fSlope*(Track2.fStartTimeInRunC-Track1.fEndTimeInRunC) - Track2.fStartFrequency)<fFrequencyAcceptance;
        bool slope_condition2 = std::abs(Track2.fStartFrequency-Track2.fSlope*(Track2.fStartTimeInRunC-Track1.fEndTimeInRunC) - Track1.fEndFrequency)<fFrequencyAcceptance;
        bool time_gap_in_line = Track1.fEndTimeInRunC <= Track2.fStartTimeInRunC;
        bool gap_smaller_than_limit = Track2.fStartTimeInRunC - Track1.fEndTimeInRunC;

        if (time_gap_in_line and gap_smaller_than_limit and (slope_condition1 or slope_condition2))
        {
            return true;
        }

        bool slope_condition1 = std::abs(Track2.fEndFrequency+Track2.fSlope*(Track1.fStartTimeInRunC-Track2.fEndTimeInRunC) - Track1.fStartFrequency)<fFrequencyAcceptance;
        bool slope_condition2 = std::abs(Track1.fStartFrequency-Track1.fSlope*(Track1.fStartTimeInRunC-Track2.fEndTimeInRunC) - Track2.fEndFrequency)<fFrequencyAcceptance;
        bool time_gap_in_line = Track2.fEndTimeInRunC <= Track1.fStartTimeInRunC;
        bool gap_smaller_than_limit = Track1.fStartTimeInRunC - Track2.fEndTimeInRunC;

        if (time_gap_in_line and gap_smaller_than_limit and (slope_condition1 or slope_condition2))
        {
            return true;
        }

    return false;
    }

    bool KTIterativeTrackClustering::DoTheyOverlap(KTProcessedTrackData& Track1, KTProcessedTrackData& Track2)
    {
        // if the start time of track 2 is between start and end time of track 1
        bool condition1 = Track1.fEndTimeInRunC > Track2.fStartTimeInRunC and Track1.fStartTimeInRunC < Track2.fStartTimeInRunC;

        // and the start frequency of track 2 is too close to track 1
        bool condition2 = std::abs(Track2.fStartFrequency - Track1.fStartFrequency + Track1.fSlope * (Track2.fStartTimeInRunC - Track1.fStartTimeInRunC)) < fFrequencyAcceptance;

        if (condition1 or condition2)
        {
            return true;
        }
        // the other way around
        bool condition3 = Track2.fEndTimeInRunC > Track1.fStartTimeInRunC and Track2.fStartTimeInRunC < Track1.fStartTimeInRunC;
        bool condition4 = std::abs(Track1.fStartFrequency - Track2.fStartFrequency + Track2.fSlope * (Track1.fStartTimeInRunC - Track2.fStartTimeInRunC)) < fFrequencyAcceptance;

        if (condition3 or condition4)
        {
            return true;
        }

        // same for end point of track2
        bool condition1 = Track1.fEndTimeInRunC > Track2.fEndTimeInRunC and Track1.fStartTimeInRunC < Track2.fEndTimeInRunC;

        // and the start frequency of track 2 is too close to track 1
        bool condition2 = std::abs(Track2.fEndFrequency - Track1.fStartFrequency + Track1.fSlope * (Track2.fEndTimeInRunC - Track1.fStartTimeInRunC)) < fFrequencyAcceptance;

        if (condition1 or condition2)
        {
            return true;
        }
        // the other way around
        bool condition3 = Track2.fEndTimeInRunC > Track1.fEndTimeInRunC and Track2.fStartTimeInRunC < Track1.fEndTimeInRunC;
        bool condition4 = std::abs(Track1.fEndFrequency - Track2.fStartFrequency + Track2.fSlope * (Track1.fEndTimeInRunC - Track2.fStartTimeInRunC)) < fFrequencyAcceptance;

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
        KTINFO(itclog, "Clustering done. Emitting new tracks followed by clustering-done signal")
        for (std::vector<KTProcessedTrackData>::const_iterator trackIt = fCompTracks.begin(); trackIt!=fCompTracks.end(); trackIt++)
        {
            bool LineIsTrack = true;

            if (fApplyPowerCut)
            {
                if (trackIt->fTotalPower <= fPowerThreshold)
                {
                    LineIsTrack = false;
                }
            }
            if (fApplyDensityCut)
            {
                if (trackIt->fTotalPower/(trackIt->fEndTimeInRunC-trackIt->fStartTimeInRunC) <= fDensityThreshold)
                {
                    LineIsTrack = false;
                }
            }

            if (LineIsTrack == true)
            {
                // Set up new data object
                Nymph::KTDataPtr data( new Nymph::KTData() );
                KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
                newTrack.SetComponent( trackIt->fComponent );
                newTrack.SetTrackID(fNTracks);
                fNTracks++;

                newTrack.SetStartTimeInRunC( trackIt->fStartTimeInRunC );
                newTrack.SetEndTimeInRunC( trackIt->fEndTimeInRunC );
                newTrack.SetStartTimeInAcq( trackIt->fStartTimeInAcq);
                newTrack.SetStartFrequency( trackIt->fStartFrequency );
                newTrack.SetEndFrequency( trackIt->fEndFrequency );
                newTrack.SetSlope(trackIt->fSlope);


                // Process & emit new track

                KTINFO(itclog, "Now processing PreCandidates");
                ProcessNewTrack( newTrack );

                KTDEBUG(itclog, "Emitting track signal");
                fTrackSignal( data );
            }
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
