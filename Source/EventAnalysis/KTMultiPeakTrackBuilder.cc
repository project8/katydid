/*
 * KTMultiPeakTrackBuilder.cc
 *
 *  Created on: Dec 5, 2016
 *      Author: E. Zayas
 */

#include "KTMultiPeakTrackBuilder.hh"

#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTMultiTrackEventData.hh"

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
    KTLOGGER(tclog, "KTMultiPeakTrackBuilder");

    KT_REGISTER_PROCESSOR(KTMultiPeakTrackBuilder, "multi-peak-track-builder");

    KTMultiPeakTrackBuilder::KTMultiPeakTrackBuilder(const std::string& name) :
            KTPrimaryProcessor(name),
            fSidebandTimeTolerance(0.),
            fTimeBinWidth(1),
            fFreqBinWidth(1.),
            fCurrentAcquisitionID(std::numeric_limits<uint64_t>::max()),
            fCompTracks(1),
            fMPTracks(1),
            fMPTSignal("mpt", this),
            fDoneSignal("mpt-done", this),
            fTakeTrackSlot("track", this, &KTMultiPeakTrackBuilder::TakeTrack)
    {
        RegisterSlot("do-clustering", this, &KTMultiPeakTrackBuilder::DoClusteringSlot);
    }

    KTMultiPeakTrackBuilder::~KTMultiPeakTrackBuilder()
    {
    }

    bool KTMultiPeakTrackBuilder::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSidebandTimeTolerance(node->get_value("sideband-time-tol", GetSidebandTimeTolerance()));

        return true;
    }

    bool KTMultiPeakTrackBuilder::TakeTrack(KTProcessedTrackData& track)
    {
        if (track.GetAcquisitionID() != fCurrentAcquisitionID)
        {
            KTINFO(tclog, "Incoming track has a new acquisition ID (new: " << track.GetAcquisitionID() << "; current: " << fCurrentAcquisitionID << "). Will do clustering for the current acquisition.");
            if (! DoClustering())
            {
                KTERROR(tclog, "An error occurred while running the event clustering");
                return false;
            }
            fCurrentAcquisitionID = track.GetAcquisitionID();
            KTDEBUG(tclog, "New acquisition ID: " << fCurrentAcquisitionID);
        }

        // ignore the track if it's been cut
        if (track.GetIsCut()) return true;

        // verify that we have the right number of components
        if (track.GetComponent() >= fCompTracks.size())
        {
            SetNComponents(track.GetComponent() + 1);
        }

        // copy the full track data
        fCompTracks[track.GetComponent()].insert(track);

        KTDEBUG(tclog, "Taking track: (" << track.GetStartTimeInRunC() << ", " << track.GetStartFrequency() << ", " << track.GetEndTimeInRunC() << ", " << track.GetEndFrequency() << ")");

        return true;
    }

    void KTMultiPeakTrackBuilder::DoClusteringSlot()
    {
        if (! Run())
        {
            KTERROR(tclog, "An error occurred while running the multi-peak track clustering");
        }
        return;
    }

    bool KTMultiPeakTrackBuilder::Run()
    {
        return DoClustering();
    }

    bool KTMultiPeakTrackBuilder::DoClustering()
    {
        if (! FindMultiPeakTracks())
        {
            KTERROR(tclog, "An error occurred while identifying multi-peak tracks");
            return false;
        }

        KTDEBUG(tclog, "MPT building complete");
        fDoneSignal();

        fMPTracks.clear();
        fMPTracks.resize(1);

        fCompTracks.clear();
        fCompTracks.resize(1);

        return true;
    }

    bool KTMultiPeakTrackBuilder::FindMultiPeakTracks()
    {
        KTINFO(tclog, "Collecting lines into multi-peak tracks");

        // loop over components
        unsigned component = 0;
        for (vector< TrackSet >::const_iterator compIt = fCompTracks.begin(); compIt != fCompTracks.end(); ++compIt)
        {
            KTDEBUG(tclog, "Doing component: " << compIt - fCompTracks.begin() + 1 << "/" << fCompTracks.size());
            fMPTracks[component].clear();

            // loop over individual tracks
            TrackSetCIt trackIt = compIt->begin();
            if (trackIt == compIt->end())
            {
                KTDEBUG(tclog, "No tracks to see here, moving right along");
                continue;
            }

            list< MultiPeakTrackRef > activeTrackRefs;

            int trackCount = 0;
            while (trackIt != compIt->end())
            {
                KTDEBUG(tclog, "considering track (" << ++trackCount << "/" << compIt->size() << ")");
                // loop over active track refs
                list< MultiPeakTrackRef >::iterator mptrIt = activeTrackRefs.begin();
                bool trackHasBeenAdded = false; // this will allow us to check all of the track refs for whether they're still active, even after adding the track to a ref
                int activeTrackCount = 0;
                while (mptrIt != activeTrackRefs.end())
                {
                    KTDEBUG(tclog, "checking active track (" << ++activeTrackCount << "/" << activeTrackRefs.size() << ")" );
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

            // emit MPTrack signals from fMPTracks
            for( std::set< MultiPeakTrackRef, MTRComp >::iterator mptData = fMPTracks[component].begin(); mptData != fMPTracks[component].end(); ++mptData )
            {
                Nymph::KTDataPtr data(new Nymph::KTData());
                KTMultiPeakTrackData& newData = data->Of< KTMultiPeakTrackData >();

                newData.SetComponent( component );
                newData.SetMPTrack( *mptData );

                fMPTSignal( data );
            }

            ++component;
        } // for loop over components

        return true;
    }

    void KTMultiPeakTrackBuilder::SetNComponents(unsigned nComps)
    {
        int fCTSize = fCompTracks.size();
        TrackSet blankSet;
        for( int i = fCTSize; i <= nComps; ++i )
            fCompTracks.push_back( blankSet );
    }

} /* namespace Katydid */
