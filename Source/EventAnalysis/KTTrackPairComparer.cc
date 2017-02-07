/*
 * KTTrackPairComparer.cc
 *
 *  Created on: Feb 7, 2017
 *      Author: E. Zayas
 */

#include "KTTrackPairComparer.hh"

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
    KTLOGGER(tclog, "KTTrackPairComparer");

    KT_REGISTER_PROCESSOR(KTTrackPairComparer, "track-pair-comparer");

    KTTrackPairComparer::KTTrackPairComparer(const std::string& name) :
            KTPrimaryProcessor(name),
            fCompTracks(1),
            fMTCompSignal("mt-comp", this),
            fDoneSignal("comparisons-done", this),
            fTakeTrackSlot("track", this, &KTTrackPairComparer::TakeTrack)
    {
        RegisterSlot("do-comparisons", this, &KTTrackPairComparer::DoComparisonsSlot);
    }

    KTTrackPairComparer::~KTTrackPairComparer()
    {
    }

    bool KTTrackPairComparer::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        return true;
    }

    bool KTTrackPairComparer::TakeTrack(KTProcessedTrackData& track)
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

    void KTTrackPairComparer::DoComparisonsSlot()
    {
        if (! Run())
        {
            KTERROR(tclog, "An error occurred while comparing pairs of tracks");
        }
        return;
    }

    bool KTTrackPairComparer::Run()
    {
        return DoComparisons();
    }

    bool KTTrackPairComparer::DoComparisons()
    {
        if (! CompareTracks())
        {
            KTERROR(tclog, "An error occurred while trying to compare tracks");
            return false;
        }

        KTDEBUG(tclog, "Track comparisons complete");
        fDoneSignal();

        return true;
    }

    bool KTTrackPairComparer::CompareTracks()
    {
        int trackIndex1 = 0, trackIndex2 = 0; // indices of a track in a loop through the set

        // parameters to calculate

        double deltaF = 0.;
        double oldPower = 0.;
        double newPower = 0.;
        double deltaQ = 0.;
        double deltaTHead = 0.;
        double deltaTTail = 0.;

        for( int iComponent = 0; iComponent < fCompTracks.size(); ++iComponent )
        {   // loop over components

            trackIndex1 = 0;
            for( TrackSetCIt trackIt = fCompTracks[iComponent].begin(); trackIt != fCompTracks[iComponent].end(); ++trackIt )
            {
                trackIndex2 = 0;
                for( TrackSetCIt secondTrackIt = fCompTracks[iComponent].begin(); secondTrackIt != fCompTracks[iComponent].end(); ++secondTrackIt )
                {
                    // now we can do a pair-wise comparison

                    // no need to compare a track with itself
                    if( trackIndex1 == trackIndex2 )
                    {
                        ++trackIndex2;
                        ++secondTrackIt;
                        continue;
                    }

                    // create data object
                    Nymph::KTDataPtr data( new Nymph::KTData() );
                    KTMultiTrackComparisonData& newData = data->Of< KTMultiTrackComparisonData >();

                    // fill component and tracks
                    newData.SetComponent( iComponent );
                    newData.AddToMPTrack( trackIt );
                    newData.SetSingleTrack( *secondTrackIt );

                    // calculate frequency difference
                    deltaF = secondTrackIt->GetStartFrequency() - trackIt->GetStartFrequency();

                    // calculate powers
                    oldPower = trackIt->GetTotalPower();
                    newPower = secondTrackIt->GetTotalPower();

                    // calculate slope difference
                    deltaQ = secondTrackIt->GetSlope() - trackIt->GetSlope();

                    // calculate track start/end differences
                    deltaTHead = secondTrackIt->GetStartTimeInRunC() - trackIt->GetStartTimeInRunC();
                    deltaTTail = secondTrackIt->GetEndTimeInRunC() - trackIt->GetEndTimeInRunC();

                    // fill calculations into data object

                    newData.SetMinDeltaFRatio( deltaF );
                    newData.SetMaxDeltaFRatio( deltaF );

                    newData.SetMeanPowerOld( oldPower );
                    newData.SetMinPowerOld( oldPower );
                    newData.SetMaxPowerOld( oldPower );

                    newData.SetPowerNew( newPower );

                    newData.SetMeanDeltaQ( deltaQ );
                    newData.SetMinDeltaQ( deltaQ );
                    newData.SetMaxDeltaQ( deltaQ );

                    newData.SetMeanDeltaTHead( deltaTHead );
                    newData.SetMinDeltaTHead( deltaTHead );
                    newData.SetMaxDeltaTHead( deltaTHead );

                    newData.SetMeanDeltaTTail( deltaTTail );
                    newData.SetMinDeltaTTail( deltaTTail );
                    newData.SetMaxDeltaTTail( deltaTTail );

                    // Emit signal
                    fMTCompSignal( data );

                    // Increment index
                    ++trackIndex2;

                } // end loop over secondTrackIt

                // Increment index
                ++trackIndex1;

            } // end loop over trackIt

        }  // end loop over components

        // Emit done signal
        fDoneSignal();

        return true;
    }

    void KTTrackPairComparer::SetNComponents(unsigned nComps)
    {
        int fCTSize = fCompTracks.size();
        TrackSet blankSet;
        for( int i = fCTSize; i <= nComps; ++i )
            fCompTracks.push_back( blankSet );
    }

} /* namespace Katydid */
