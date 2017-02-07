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
