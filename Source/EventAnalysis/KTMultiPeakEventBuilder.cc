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
            fJumpFreqTolerance(1.),
            fTimeBinWidth(1),
            fFreqBinWidth(1.),
            fCompTracks(1),
            fMPTracks(1),
            fCandidates(),
            fDataCount(0),
            fEventSignal("event", this),
            fEventsDoneSignal("clustering-done", this),
            fTakeTrackSlot("track", this, &KTMultiPeakEventBuilder::TakeTrack)
    //        fDoClusterSlot("do-cluster-trigger", this, &KTMultiPeakEventBuilder::Run)
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
        SetJumpFreqTolerance(node->GetValue("jump-freq-tol", GetJumpFreqTolerance()));

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
        // loop over components
        unsigned component = 0;
        for (vector< TrackSet >::const_iterator compIt = fCompTracks.begin(); compIt != fCompTracks.end(); ++compIt)
        {
            fMPTracks[component].clear();

            // loop over individual tracks
            TrackSetCIt trackIt = compIt->begin();
            if (trackIt == compIt->end()) continue;

            list< MultiPeakTrackRef > activeTrackRefs;
            activeTrackRefs.push_back(MultiPeakTrackRef());
            activeTrackRefs.begin()->InsertTrack(trackIt);

            ++trackIt;
            while (trackIt != compIt->end())
            {
                // loop over active track refs
                list< MultiPeakTrackRef >::iterator mptrIt = activeTrackRefs.begin();
                bool trackHasBeenAdded = false; // this will allow us to check all of the track refs for whether they're still active, even after adding the track to a ref
                while (mptrIt != activeTrackRefs.end())
                {
                    double deltaStartT = trackIt->GetStartTimeInRunC() - mptrIt->fMeanStartTimeInRunC;

                    // check to see if this track ref should no longer be active
                    if (trackIt->GetStartTimeInRunC() - mptrIt->fMeanStartTimeInRunC > fSidebandTimeTolerance)
                    {
                        // there's no way this track, or any following it in the set, will match in time
                        fMPTracks[component].insert(*mptrIt);
                        mptrIt = activeTrackRefs.erase(mptrIt); // this results in mptrIt being one element past the one that was erased
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
        KTPROG(tclog, "KTMultiPeakEventBuilder combining multi-peak tracks");
      
        // we're unpacking all components into a unified set of events, so this goes outside the loop
        // also, I've given no thought to the use of std::vector vs std::list or std::forward_list
        typedef std::pair< KTDataPtr, std::vector< double > > ActiveEventType; // without std::pair because not tracking frequencies
        //typedef std::pair< KTDataPtr, std::vector< std::pair< double, double > > > ActiveEventType;
        std::vector< ActiveEventType > active_events;

        // loop over components
        for (unsigned iComponent = 0; iComponent < fMPTracks.size(); ++iComponent)
        {
            KTDEBUG(tclog, "head-to-tail building events from MPTracks; component: " << iComponent);
            
            if (fMPTracks[iComponent].empty())
            {
                continue;
            }
            std::set< MultiPeakTrackRef, MTRComp >::iterator trackIt = fMPTracks[iComponent].begin();

            // loop over new Multi-Peak Tracks
            while (trackIt != fMPTracks[iComponent].end())
            {
                int track_assigned = -1; // keep track of which event the track when into
                
                // loop over active events and add this track to one
                for (std::vector< ActiveEventType >::iterator EventIt=active_events.begin(); EventIt != active_events.end();)
                {
                    bool increment_eventIt = true;
                    // loop over track ends to test against
                    // TODO: track ends vector should be a set, that way random insertions maintain their order
                    for (std::vector< double >::iterator end_timeIt=EventIt->second.begin(); end_timeIt != EventIt->second.end();)
                    {
                        // if this track head matches the tail of a track in this event, add it
                        // TODO:<upgrade> there should (maybe) be a freq jump limit too but for MP tracks how is that def'd? Is it relative to the mean? to the closest track?
                        if ( trackIt->fMeanEndTimeInRunC - *end_timeIt < fJumpTimeTolerance )
                        {
                            if (track_assigned == -1) // If this track hasn't been added to any event, add to this one
                            {
                                track_assigned = end_timeIt - EventIt->second.begin();

                                KTMultiTrackEventData& this_event = EventIt->first->Of< KTMultiTrackEventData >();
                                for ( std::set< TrackSetCIt, TrackSetCItComp >::iterator peakIt=trackIt->fTrackRefs.begin(); peakIt != trackIt->fTrackRefs.end(); ++peakIt )
                                {
                                    this_event.AddTrack( **peakIt );
                                }
                                EventIt->second.push_back( trackIt->fMeanEndTimeInRunC );
                            }
                            else // if this track is already in an event, merge this event into that one (NOTE: this is weird)
                            {
                                std::vector< ActiveEventType >::iterator first_event_loc = active_events.begin();
                                std::advance( first_event_loc, track_assigned);
                                KTMultiTrackEventData& first_event = first_event_loc->first->Of< KTMultiTrackEventData >();
                                KTMultiTrackEventData& this_event = EventIt->first->Of< KTMultiTrackEventData >();
                                this_event.SetUnknownEventTopology(true);
                                for (unsigned iLine = 0; iLine < this_event.GetNTracks(); ++iLine)
                                {
                                    first_event.AddTrack(this_event.GetTrack(iLine));
                                }
                                for (std::vector< double >::const_iterator endpointIt=EventIt->second.begin(); endpointIt != EventIt->second.end(); ++endpointIt)
                                {
                                    first_event_loc->second.push_back(*endpointIt);
                                }
                                // because we merged this event into an earlier one, we should remove it (and not increment the iterator)
                                EventIt = active_events.erase(EventIt);
                                increment_eventIt = false;
                            }
                            // since this track already matches this event, we don't keep testing track ends in the event
                            break;
                        }
                        ++end_timeIt;
                    } // for loop over end times
                    // don't increment if we removed this active event
                    if (increment_eventIt)
                    {
                        ++EventIt;
                    }
                } // for loop over active events
                if (track_assigned != -1) // if no event matched then create one
                {
                    ActiveEventType new_event;
                    KTMultiTrackEventData& event = new_event.first->Of< KTMultiTrackEventData >();
                    // TODO: I probably need to set some attributes of the KTDataPtr
                    // TODO: Then set some KTMultiTrackEventData attributes
                    // TODO: Then loop over and add tracks in the MP Track
                    // TODO: And set any attributes of those tracks
                    // TODO: Also, add the track end time to the end times vector <pair>.second
                    active_events.push_back(new_event);
                }
                ++trackIt;
            } // while loop over tracks
        } // for loop over components
        
        // TODO: move acitve tracks into fCandidates
        // TODO: emit signals?
        // TODO:<upgrade> put logic to move active events into fCandidates into loop on active events, that way events that are closed aren't tested every time, also the signal can go ahead and by passed along

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
            fSumEndTimeInRunC(0.)
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
