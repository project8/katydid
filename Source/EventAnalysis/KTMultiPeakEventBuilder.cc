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
#include "KTParam.hh"
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
            //fJumpFreqTolerance(1.),
            fTimeBinWidth(1),
            fFreqBinWidth(1.),
            fCompTracks(1),
            fMPTracks(1),
            fCandidates(),
            fDataCount(0),
            fEventSignal("event", this),
            fEventsDoneSignal("events-done", this),
            fTakeTrackSlot("track", this, &KTMultiPeakEventBuilder::TakeTrack)
    {
        RegisterSlot("do-clustering", this, &KTMultiPeakEventBuilder::DoClusteringSlot);
    }

    KTMultiPeakEventBuilder::~KTMultiPeakEventBuilder()
    {
    }

    bool KTMultiPeakEventBuilder::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        SetSidebandTimeTolerance(node->GetValue("sideband-time-tol", GetSidebandTimeTolerance()));
        SetJumpTimeTolerance(node->GetValue("jump-time-tol", GetJumpTimeTolerance()));
        //SetJumpFreqTolerance(node->GetValue("jump-freq-tol", GetJumpFreqTolerance()));

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

    /*
    bool KTMultiPeakEventBuilder::TakePoint(double time, double frequency *//*, double amplitude*//*, unsigned component)
    {
        if (component >= fCompPoints.size())
        {
            SetNComponents(component + 1);
        }

        KTDBScan::Point newPoint(fNDimensions);
        newPoint(0) = time;
        newPoint(1) = frequency;
        fCompPoints[component].push_back(newPoint);

        KTDEBUG(tclog, "Point " << fCompPoints[component].size()-1 << " is now " << fCompPoints[component].back());

        return true;
    }
    */

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
            //activeTrackRefs.push_back(MultiPeakTrackRef());
            //activeTrackRefs.begin()->InsertTrack(trackIt);
            //++trackIt;

            int trackCount = 0;
            while (trackIt != compIt->end())
            {
                KTINFO(tclog, "considering track (" << ++trackCount << "/" << compIt->size() << ")");
                // loop over active track refs
                KTERROR(tclog, "new track is from acq: " << trackIt->GetAcquisitionID())
                list< MultiPeakTrackRef >::iterator mptrIt = activeTrackRefs.begin();
                bool trackHasBeenAdded = false; // this will allow us to check all of the track refs for whether they're still active, even after adding the track to a ref
                int activeTrackCount = 0;
                while (mptrIt != activeTrackRefs.end())
                {
                    KTINFO(tclog, "checking active track (" << ++activeTrackCount << "/" << activeTrackRefs.size() << ")" );
                    double deltaStartT = trackIt->GetStartTimeInRunC() - mptrIt->fMeanStartTimeInRunC;

                    // check to see if this track ref should no longer be active
                    if (trackIt->GetStartTimeInRunC() - mptrIt->fMeanStartTimeInRunC > fSidebandTimeTolerance)
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
                        if (! trackHasBeenAdded && fabs(deltaStartT) <= fSidebandTimeTolerance && fabs(deltaEndT) < fSidebandTimeTolerance)
                        {
                            // then this track matches this track ref
                            mptrIt->InsertTrack(trackIt);
                            trackHasBeenAdded = true;
                        }


                        ++mptrIt;
                    }
                } // while loop over active track refs
                if (! trackHasBeenAdded)
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

        // loop over components
        for (unsigned iComponent = 0; iComponent < fMPTracks.size(); ++iComponent)
        {
            KTINFO(tclog, "Doing component: (" << iComponent + 1 << "/" << fCompTracks.size() << ")");
            // if the component has no tracks, skip it
            if (fMPTracks[iComponent].empty())
            {
                continue;
            }
            std::set< MultiPeakTrackRef, MTRComp >::iterator trackIt = fMPTracks[iComponent].begin();

            // loop over new Multi-Peak Tracks
            int countMPTracks = 0;
            while (trackIt != fMPTracks[iComponent].end())
            {
                KTINFO(tclog, "placing MPTrack (" << ++countMPTracks << "/" << fMPTracks[iComponent].size() << ")");
                int trackAssigned = -1; // keep track of which event the track when into

                // loop over active events and add this track to one
                for (std::vector< ActiveEventType >::iterator eventIt=activeEvents.begin(); eventIt != activeEvents.end();)
                {
                    KTDEBUG(tclog, "checking active event (" << eventIt - activeEvents.begin() + 1 << "/" << activeEvents.size() << ")");
                    bool incrementEventIt = true;
                    // if the event's last end is earlier than this track's start, the event is done
                    if ( trackIt->fMeanStartTimeInRunC - fJumpTimeTolerance > *(eventIt->second.rbegin()) )
                    {
                        KTDEBUG(tclog, "event no longer active");
                        fCandidates.insert(eventIt->first);
                        eventIt = activeEvents.erase(eventIt);
                        incrementEventIt = false;
                        continue;
                    }
                    // loop over track ends to test against
                    for (TrackEndsType::iterator endTimeIt=eventIt->second.begin(); endTimeIt != eventIt->second.end();)
                    {
                        // if this track head matches the tail of a track in this event, add it
                        if ( trackIt->fMeanEndTimeInRunC - *endTimeIt < fJumpTimeTolerance )
                        {
                            KTDEBUG(tclog, "track matched this active event");
                            if (trackAssigned == -1) // If this track hasn't been added to any event, add to this one
                            {
                                trackAssigned = eventIt - activeEvents.begin();

                                KTMultiTrackEventData& thisEvent = eventIt->first->Of< KTMultiTrackEventData >();
                                for ( std::set< TrackSetCIt, TrackSetCItComp >::iterator peakIt=trackIt->fTrackRefs.begin(); peakIt != trackIt->fTrackRefs.end(); ++peakIt )
                                {
                                    thisEvent.AddTrack( **peakIt );
                                }
                                thisEvent.ProcessTracks();
                                eventIt->second.insert( trackIt->fMeanEndTimeInRunC );
                            }
                            else // if this track is already in an event, merge this event into that one (NOTE: this is weird)
                            {
                                std::vector< ActiveEventType >::iterator firstEventLoc = activeEvents.begin();
                                std::advance( firstEventLoc, trackAssigned);
                                KTMultiTrackEventData& firstEvent = firstEventLoc->first->Of< KTMultiTrackEventData >();
                                KTMultiTrackEventData& thisEvent = eventIt->first->Of< KTMultiTrackEventData >();
                                thisEvent.SetUnknownEventTopology(true);
                                for (unsigned iLine = 0; iLine < thisEvent.GetNTracks(); ++iLine)
                                {
                                    firstEvent.AddTrack(thisEvent.GetTrack(iLine));
                                }
                                firstEvent.ProcessTracks();
                                for (TrackEndsType::const_iterator endpointIt=eventIt->second.begin(); endpointIt != eventIt->second.end(); ++endpointIt)
                                {
                                    firstEventLoc->second.insert(*endpointIt);
                                }
                                eventIt = activeEvents.erase(eventIt);
                                incrementEventIt = false;
                            }
                            // this track already matched the event, don't keep checking
                            break;
                        }
                        ++endTimeIt;
                    } // for loop over end times
                    // don't increment if we removed this active event from the vector
                    if (incrementEventIt)
                    {
                        ++eventIt;
                    }
                } // for loop over active events
                if (trackAssigned == -1) // if no event matched then create one
                {
                    KTINFO(tclog, "track not matched, creating new event");
                    KTDataPtr data(new KTData());
                    ActiveEventType new_event(data, TrackEndsType());
                    KTMultiTrackEventData& event = new_event.first->Of< KTMultiTrackEventData >();
                    event.SetComponent(iComponent);
                    event.SetAcquisitionID(trackIt->fAcquisitionID);
                    for ( std::set< TrackSetCIt, TrackSetCItComp >::iterator peakIt=trackIt->fTrackRefs.begin(); peakIt != trackIt->fTrackRefs.end(); ++peakIt )
                    {
                        event.AddTrack( **peakIt );
                    }
                    event.ProcessTracks();
                    new_event.second.insert( trackIt->fMeanEndTimeInRunC );
                    activeEvents.push_back(new_event);
                    //KTINFO(tclog, "track not matched, creating active event " << activeEvents.size());
                }
                else
                {
                    KTINFO(tclog, "track assigned to event " << trackAssigned);
                }
                ++trackIt;
            } // while loop over tracks
        } // for loop over components

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

    KTMultiPeakEventBuilder::MultiPeakTrackRef::MultiPeakTrackRef() :
            fTrackRefs(),
            fMeanStartTimeInRunC(0.),
            fSumStartTimeInRunC(0.),
            fMeanEndTimeInRunC(0.),
            fSumEndTimeInRunC(0.),
            fAcquisitionID(0)
    {}

    bool KTMultiPeakEventBuilder::MultiPeakTrackRef::InsertTrack(const TrackSetCIt& trackRef)
    {
        if (fTrackRefs.find(trackRef) != fTrackRefs.end())  return false;

        fTrackRefs.insert(trackRef);
        fSumStartTimeInRunC += trackRef->GetStartTimeInRunC();
        fSumEndTimeInRunC += trackRef->GetEndTimeInRunC();
        double currentSize = (double)fTrackRefs.size();
        fMeanStartTimeInRunC = fSumStartTimeInRunC / currentSize;
        fMeanEndTimeInRunC = fSumEndTimeInRunC / currentSize;
        return true;
    }


} /* namespace Katydid */
