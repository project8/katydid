/*
 * KTMultiPeakEventBuilder.cc
 *
 *  Created on: Dec 7, 2015
 *      Author: N.S. Oblath
 */

#include "KTMultiPeakEventBuilder.hh"

#include "KTLogger.hh"
#include "KTMath.hh"

#include <list>

#ifndef NDEBUG
#include <sstream>
#endif

using std::list;
using std::set;
using std::vector;

namespace Katydid
{
    KTLOGGER(tclog, "KTMultiPeakEventBuilder");

    KT_REGISTER_PROCESSOR(KTMultiPeakEventBuilder, "multi-peak-event-builder");

    KTMultiPeakEventBuilder::KTMultiPeakEventBuilder(const std::string& name) :
            KTPrimaryProcessor(name),
            fJumpTimeTolerance(0.),
            fTimeBinWidth(1),
            fFreqBinWidth(1.),
            fCurrentAcquisitionID(std::numeric_limits<uint64_t>::max()),
            fMPTracks(1),
            fCandidates(),
            fDataCount(-1),
            fEventSignal("event", this),
            fMPTSignal("mpt", this),
            fEventsDoneSignal("events-done", this),
            fTakeMPTSlot("mpt", this, &KTMultiPeakEventBuilder::TakeMPT)
    {
        RegisterSlot("do-clustering", this, &KTMultiPeakEventBuilder::DoClusteringSlot);
    }

    KTMultiPeakEventBuilder::~KTMultiPeakEventBuilder()
    {
    }

    bool KTMultiPeakEventBuilder::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetJumpTimeTolerance(node->get_value("jump-time-tol", GetJumpTimeTolerance()));

        return true;
    }

    bool KTMultiPeakEventBuilder::TakeMPT(KTMultiPeakTrackData& mpt)
    {
        if (mpt.GetAcquisitionID() != fCurrentAcquisitionID)
        {
            KTINFO(tclog, "Incoming MPT has a new acquisition ID (new: " << mpt.GetAcquisitionID() << "; current: " << fCurrentAcquisitionID << "). Will do clustering for the current acquisition.");
            if (! DoClustering())
            {
                KTERROR(tclog, "An error occurred while running the event clustering");
                return false;
            }
            fCurrentAcquisitionID = mpt.GetAcquisitionID();
            KTDEBUG(tclog, "New acquisition ID: " << fCurrentAcquisitionID);
        }

        // verify that we have the right number of components
        if (mpt.GetComponent() >= fMPTracks.size())
        {
            SetNComponents(mpt.GetComponent() + 1);
        }

        KTDEBUG(tclog, "Taking MPT: (" << mpt.GetMeanStartTimeInRunC() << ", " << mpt.GetMeanEndTimeInRunC() << ")");

        // copy the full track data
        MultiPeakTrackRef mptr = mpt.GetMPTrack();
        fMPTracks[mpt.GetComponent()].insert(mptr);

        return true;
    }

    void KTMultiPeakEventBuilder::DoClusteringSlot()
    {
        if (! Run())
        {
            KTERROR(tclog, "An error occurred while running the event clustering");
        }
        return;
    }

    bool KTMultiPeakEventBuilder::Run()
    {
        return DoClustering();
    }

    bool KTMultiPeakEventBuilder::DoClustering()
    {

        if (! FindEvents())
        {
            KTERROR(tclog, "An error occurred while identifying events");
            return false;
        }

        KTDEBUG(tclog, "Event building complete");
        fEventsDoneSignal();

        fCandidates.clear();

        fMPTracks.clear();
        fMPTracks.resize(1);

        return true;
    }

    bool KTMultiPeakEventBuilder::FindEvents()
    {
        KTINFO(tclog, "Combining multi-peak tracks into events");

        // we're unpacking all components into a unified set of events, so this goes outside the loop
        typedef std::set< double > TrackEndsType;
        typedef std::pair< Nymph::KTDataPtr, TrackEndsType > ActiveEventType;
        std::vector< ActiveEventType > activeEvents;

        for (unsigned iComponent = 0; iComponent < fMPTracks.size(); ++iComponent)
        { // loop over components
            KTDEBUG(tclog, "Doing component: (" << iComponent + 1 << "/" << fMPTracks.size() << ")");
            if (fMPTracks[iComponent].empty())
            { // if the component has no tracks, skip it
                KTDEBUG(tclog, "No MPTs to cluster");
                continue;
            }
            std::set< MultiPeakTrackRef, MTRComp >::iterator trackIt = fMPTracks[iComponent].begin();

            int countMPTracks = 0;
            while (trackIt != fMPTracks[iComponent].end())
            { // loop over new Multi-Peak Tracks
                KTDEBUG(tclog, "placing MPTrack (" << ++countMPTracks << "/" << fMPTracks[iComponent].size() << ")");
                int trackAssigned = -1; // keep track of which event the track when into

                for (std::vector< ActiveEventType >::iterator eventIt=activeEvents.begin(); eventIt != activeEvents.end();)
                { // loop over active events and add this track to one
                    KTDEBUG(tclog, "checking active event (" << eventIt - activeEvents.begin() + 1 << "/" << activeEvents.size() << ")");
                    bool incrementEventIt = true;
                    if ( trackIt->fMeanStartTimeInRunC - fJumpTimeTolerance > *(eventIt->second.rbegin()) )
                    { // if the event's last end is earlier than this track's start, the event is done
                        KTDEBUG(tclog, "event no longer active");
                        eventIt->first->Of< KTMultiTrackEventData >().ProcessTracks();
                        fCandidates.insert(eventIt->first);
                        eventIt = activeEvents.erase(eventIt);
                        incrementEventIt = false;
                        continue;
                    }
                    for (TrackEndsType::iterator endTimeIt=eventIt->second.begin(); endTimeIt != eventIt->second.end();)
                    { // loop over track ends to test against
                        if ( std::abs( trackIt->fMeanStartTimeInRunC - *endTimeIt ) < fJumpTimeTolerance )
                        { // if this track head matches the tail of a track in this event, add it

                            // The comparison logic here allows a "gap" or an "overlap" up to the
                            // tolerance; we could make this more specific (i.e. allow only gap OR
                            // overlap, or have different tolerances for each) if the need arises

                            KTDEBUG(tclog, "track matched this active event");
                            if (trackAssigned == -1)
                            { // If this track hasn't been added to any event, add to this one
                                trackAssigned = eventIt - activeEvents.begin();

                                KTMultiTrackEventData& thisEvent = eventIt->first->Of< KTMultiTrackEventData >();
                                thisEvent.AddTracks(trackIt->fTrackRefs);
                                if (trackIt->fUnknownEventTopology)
                                {
                                    thisEvent.SetUnknownEventTopology(true);
                                }
                                thisEvent.ProcessTracks();
                                eventIt->second.insert( trackIt->fMeanEndTimeInRunC );
                            }
                            else
                            { // if this track is already in an event, merge this event into that one (NOTE: this is weird)
                                std::vector< ActiveEventType >::iterator firstEventLoc = activeEvents.begin();
                                std::advance( firstEventLoc, trackAssigned);
                                KTMultiTrackEventData& firstEvent = firstEventLoc->first->Of< KTMultiTrackEventData >();
                                KTMultiTrackEventData& thisEvent = eventIt->first->Of< KTMultiTrackEventData >();
                                thisEvent.SetUnknownEventTopology(true);
                                //for (unsigned iLine = 0; iLine < thisEvent.GetNTracks(); ++iLine)
                                firstEvent.AddTracks(thisEvent.GetTracksSet());
                                /*for (TrackSetCIt eventTrackIt=thisEvent.GetTracksBegin(); eventTrackIt != thisEvent.GetTracksEnd(); eventTrackIt++)
                                {
                                    firstEvent.AddTracks(eventTrackIt);//thisEvent.GetTrack(iLine));
                                    KTERROR("don't know how to deal with EventSequenceID here!!!")
                                }*/
                                firstEvent.ProcessTracks();
                                for (TrackEndsType::const_iterator endpointIt=eventIt->second.begin(); endpointIt != eventIt->second.end(); ++endpointIt)
                                {
                                    firstEventLoc->second.insert(*endpointIt);
                                }
                                eventIt = activeEvents.erase(eventIt);
                                incrementEventIt = false;
                            }
                            break; // this track already matched the event, don't keep checking
                        }
                        ++endTimeIt;
                    } // for loop over end times
                    if (incrementEventIt)
                    { // don't increment if we removed this active event from the vector
                        ++eventIt;
                    }
                } // for loop over active events
                if (trackAssigned == -1)
                { // if no event matched then create one
                    KTDEBUG(tclog, "track not matched, creating new event");
                    ++fDataCount;
                    Nymph::KTDataPtr data(new Nymph::KTData());
                    ActiveEventType new_event(data, TrackEndsType());
                    KTMultiTrackEventData& event = new_event.first->Of< KTMultiTrackEventData >();
                    event.SetComponent(iComponent);
                    event.SetAcquisitionID(trackIt->fAcquisitionID);
                    event.SetEventID(fDataCount);

                    event.AddTracks(trackIt->fTrackRefs);
                    if (trackIt->fUnknownEventTopology)
                    {
                        event.SetUnknownEventTopology(true);
                    }
                    event.ProcessTracks();
                    new_event.second.insert( trackIt->fMeanEndTimeInRunC );
                    activeEvents.push_back(new_event);
                }
                else
                {
                    KTDEBUG(tclog, "track assigned to event " << trackAssigned);
                }
                ++trackIt;
            } // while loop over tracks
        } // for loop over components

        // tracks are done so any remaining active events are done
        std::vector< ActiveEventType >::iterator eventIt=activeEvents.begin();
        while (eventIt != activeEvents.end())
        {
            eventIt->first->Of< KTMultiTrackEventData >().ProcessTracks();
            fCandidates.insert(eventIt->first);
            eventIt = activeEvents.erase(eventIt);
        }

        // emit event signals
        for (std::set< Nymph::KTDataPtr >::const_iterator dataIt=fCandidates.begin(); dataIt != fCandidates.end(); ++dataIt)
        {
            fEventSignal(*dataIt);

            // Reconstruct each MPT from the event
            KTMultiTrackEventData& mpEventData = (*dataIt)->Of< KTMultiTrackEventData >();
            TrackSet allTracks = mpEventData.GetTracksSet();

            int totalMPT = mpEventData.GetTotalEventSequences(); // total number of MPT structures
            int currentMPT = 0;

            KTDEBUG(tclog, "Total event sequences = " << totalMPT);

            std::vector< KTMultiPeakTrackData > allMPTracks; // vector to hold MPTs

            // initialize each MPTrack
            KTMultiPeakTrackData blankMPT;
            for( int i = 0; i < totalMPT; ++i )
            {
                KTDEBUG(tclog, "Initializing MPTrack #" << i);
                allMPTracks.push_back( blankMPT );
            }

            // Iterate through all tracks
            for( TrackSetCIt it = allTracks.begin(); it != allTracks.end(); ++it )
            {
                currentMPT = it->fProcTrack.GetEventSequenceID();
                KTDEBUG(tclog, "Current event sequence = " << currentMPT);
                // Fill the appropriate MPT
                allMPTracks[currentMPT].AddToMPTrack( it );
                allMPTracks[currentMPT].SetComponent( mpEventData.GetComponent() );
                allMPTracks[currentMPT].SetAcquisitionID( mpEventData.GetAcquisitionID() );
                allMPTracks[currentMPT].SetUnknownEventTopology( mpEventData.GetUnknownEventTopology() );

                // Set the event sequence ID
                allMPTracks[currentMPT].SetEventSequenceID( currentMPT );
                KTDEBUG(tclog, "MPT now has multiplicity " << allMPTracks[currentMPT].GetMultiplicity());
            }

            KTINFO(tclog, "Emitting MPT signals");

            // Construct data pointers and emit MPT signals
            for( int iMPT = 0; iMPT < totalMPT; ++iMPT )
            {
                Nymph::KTDataPtr mptData( new Nymph::KTData() );
                KTMultiPeakTrackData& newMPT = mptData->Of< KTMultiPeakTrackData >();
                newMPT = allMPTracks[iMPT];

                KTDEBUG(tclog, "Emitting MPT signal with multiplicity " << newMPT.GetMultiplicity());
                KTDEBUG(tclog, "Original MPT had multiplicity " << allMPTracks[iMPT].GetMultiplicity());

                fMPTSignal( mptData );
            }
        }
        // clear everything since we've emitted these events
        fCandidates.clear();

       return true;
    }

    void KTMultiPeakEventBuilder::SetNComponents(unsigned nComps)
    {
        int fMPTSize = fMPTracks.size();
        std::set< MultiPeakTrackRef, MTRComp > blankSet;
        for( int i = fMPTSize; i <= nComps; ++i )
            fMPTracks.push_back( blankSet );
    }

} /* namespace Katydid */
