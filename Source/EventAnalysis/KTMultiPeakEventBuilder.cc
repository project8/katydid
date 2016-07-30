/*
 * KTMultiPeakEventBuilder.cc
 *
 *  Created on: Dec 7, 2015
 *      Author: N.S. Oblath
 */

#include "KTMultiPeakEventBuilder.hh"

#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTMultiTrackEventData.hh"
#include "KTNOFactory.hh"
#include "KTProcessedTrackData.hh"

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
            fSidebandTimeTolerance(0.),
            fJumpTimeTolerance(0.),
            fTimeBinWidth(1),
            fFreqBinWidth(1.),
            fCompTracks(1),
            fMPTracks(1),
            fCandidates(),
            fDataCount(-1),
            fEventSignal("event", this),
            fEventsDoneSignal("events-done", this),
            fTakeTrackSlot("track", this, &KTMultiPeakEventBuilder::TakeTrack)
    {
        RegisterSlot("do-clustering", this, &KTMultiPeakEventBuilder::DoClusteringSlot);
    }

    KTMultiPeakEventBuilder::~KTMultiPeakEventBuilder()
    {
    }

    bool KTMultiPeakEventBuilder::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSidebandTimeTolerance(node->get_value("sideband-time-tol", GetSidebandTimeTolerance()));
        SetJumpTimeTolerance(node->get_value("jump-time-tol", GetJumpTimeTolerance()));

        return true;
    }

    bool KTMultiPeakEventBuilder::TakeTrack(KTProcessedTrackData& track)
    {
        // ignore the track if it's been cut
        if (track.GetIsCut()) return true;

        // verify that we have the right number of components
        if (track.GetComponent() >= fCompTracks.size())
        {
            SetNComponents(track.GetComponent() + 1);
        }

        // copy the full track data
        fCompTracks[track.GetComponent()].insert(track);

        KTDEBUG(tclog, "Taking track: (" << track.GetStartTimeInRunC() << ", " << track.GetStartFrequency() << ", " << track.GetEndTimeInRunC() << ", " << track.GetEndFrequency());

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
        if (! FindMultiPeakTracks())
        {
            KTERROR(tclog, "An error occurred while identifying multipeak tracks");
            return false;
        }

        if (! FindEvents())
        {
            KTERROR(tclog, "An error occurred while identifying events");
            return false;
        }

        KTDEBUG(tclog, "Event building complete");
        fEventsDoneSignal();

        return true;
    }

    bool KTMultiPeakEventBuilder::FindMultiPeakTracks()
    {
        KTPROG(tclog, "collecting lines into multi-peak tracks");
        // loop over components
        unsigned component = 0;
        for (vector< TrackSet >::const_iterator compIt = fCompTracks.begin(); compIt != fCompTracks.end(); ++compIt)
        {
            KTINFO(tclog, "Doing component: " << compIt - fCompTracks.begin() + 1 << "/" << fCompTracks.size());
            fMPTracks[component].clear();

            // loop over individual tracks
            TrackSetCIt trackIt = compIt->begin();
            if (trackIt == compIt->end()) continue;

            list< MultiPeakTrackRef > activeTrackRefs;

            int trackCount = 0;
            while (trackIt != compIt->end())
            {
                KTINFO(tclog, "considering track (" << ++trackCount << "/" << compIt->size() << ")");
                // loop over active track refs
                list< MultiPeakTrackRef >::iterator mptrIt = activeTrackRefs.begin();
                bool trackHasBeenAdded = false; // this will allow us to check all of the track refs for whether they're still active, even after adding the track to a ref
                int activeTrackCount = 0;
                while (mptrIt != activeTrackRefs.end())
                {
                    KTINFO(tclog, "checking active track (" << ++activeTrackCount << "/" << activeTrackRefs.size() << ")" );
                    double deltaStartT = trackIt->GetStartTimeInRunC() - mptrIt->fMeanStartTimeInRunC;

                    // check to see if this track ref should no longer be active
                    if (deltaStartT > fSidebandTimeTolerance)
                    {
                        KTDEBUG(tclog, "this track ref should no longer be active");
                        // there's no way this track, or any following it in the set, will match in time
                        fMPTracks[component].insert(*mptrIt);
                        mptrIt = activeTrackRefs.erase(mptrIt); // this results in mptrIt being one element past the one that was erased
                        KTDEBUG(tclog, "there are now " << fMPTracks[component].size() << " completed MPTracks");
                    }
                    else
                    {
                        double deltaEndT = trackIt->GetEndTimeInRunC() - mptrIt->fMeanEndTimeInRunC;
                        // check if this track should be added to this track ref
                        if ( !trackHasBeenAdded &&
                             (fabs(deltaStartT) <= fSidebandTimeTolerance || fabs(deltaEndT) < fSidebandTimeTolerance)
                           )
                        {
                            // then this track matches this track ref
                            mptrIt->InsertTrack(trackIt);
                            trackHasBeenAdded = true;
                            if (!(fabs(deltaStartT) <= fSidebandTimeTolerance && fabs(deltaEndT) < fSidebandTimeTolerance))
                            {
                                mptrIt->fUnknownEventTopology = true;
                            }
                        }
                        ++mptrIt; // only increment if we haven't removed one
                    }
                } // while loop over active track refs
                if (! trackHasBeenAdded) //track didn't match anything, create a new MPTrack
                {
                    activeTrackRefs.push_back(MultiPeakTrackRef());
                    activeTrackRefs.rbegin()->InsertTrack(trackIt);
                    activeTrackRefs.rbegin()->fAcquisitionID = trackIt->GetAcquisitionID();
                    trackHasBeenAdded = true;
                }
            ++trackIt;
            } // while loop over tracks

            // now that we're done with tracks, all active track refs are finished as well
            list< MultiPeakTrackRef >::iterator mptrIt = activeTrackRefs.begin();
            while (mptrIt != activeTrackRefs.end())
            {
                fMPTracks[component].insert(*mptrIt);
                mptrIt = activeTrackRefs.erase(mptrIt); // this results in mptrIt being one element past the one that was erased
            }

            ++component;
        } // for loop over components

        return true;
    }

    bool KTMultiPeakEventBuilder::FindEvents()
    {
        KTPROG(tclog, "combining multi-peak tracks into events");

        // we're unpacking all components into a unified set of events, so this goes outside the loop
        typedef std::set< double > TrackEndsType;
        typedef std::pair< KTDataPtr, TrackEndsType > ActiveEventType;
        std::vector< ActiveEventType > activeEvents;

        for (unsigned iComponent = 0; iComponent < fMPTracks.size(); ++iComponent)
        { // loop over components
            KTINFO(tclog, "Doing component: (" << iComponent + 1 << "/" << fCompTracks.size() << ")");
            if (fMPTracks[iComponent].empty())
            { // if the component has no tracks, skip it
                continue;
            }
            std::set< MultiPeakTrackRef, MTRComp >::iterator trackIt = fMPTracks[iComponent].begin();

            int countMPTracks = 0;
            while (trackIt != fMPTracks[iComponent].end())
            { // loop over new Multi-Peak Tracks
                KTINFO(tclog, "placing MPTrack (" << ++countMPTracks << "/" << fMPTracks[iComponent].size() << ")");
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
                        if ( trackIt->fMeanEndTimeInRunC - *endTimeIt < fJumpTimeTolerance )
                        { // if this track head matches the tail of a track in this event, add it
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
                    KTINFO(tclog, "track not matched, creating new event");
                    ++fDataCount;
                    KTDataPtr data(new KTData());
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
                    KTINFO(tclog, "track assigned to event " << trackAssigned);
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
        for (std::set< KTDataPtr >::const_iterator dataIt=fCandidates.begin(); dataIt != fCandidates.end(); ++dataIt)
        {
            fEventSignal(*dataIt);
        }
        // clear everything since we've emitted these events
        fCandidates.clear();
        for (unsigned iComponent = 0; iComponent < fMPTracks.size(); ++iComponent)
        {
            fCompTracks[iComponent].clear();
        }

       return true;
    }

    void KTMultiPeakEventBuilder::SetNComponents(unsigned nComps)
    {
        //TODO: fix this
        //fCompTracks.resize(nComps, vector< KTProcessedTrackData >());
        return;
    }

} /* namespace Katydid */
