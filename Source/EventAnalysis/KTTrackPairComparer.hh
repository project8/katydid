/**
 @file KTTrackPairComparer.hh
 @brief Contains KTTrackPairComparer
 @details Collects and compares all tracks
 @author: E. Zayas
 @date: Feb 7, 2017
 */

#ifndef KTTRACKPAIRCOMPARER_HH_
#define KTTRACKPAIRCOMPARER_HH_

#include "KTPrimaryProcessor.hh"

#include "KTSlot.hh"
#include "KTData.hh"
#include "KTMemberVariable.hh"
#include "KTMultiTrackEventData.hh"

#include <algorithm>
#include <set>
#include <vector>
#include <list>


namespace Katydid
{
    
    /*!
     @class KTTrackPairComparer
     @author E. Zayas

     @brief Compares the alignment in time, frequency, and slope of every pair of tracks collected

     @details
     This class does not group tracks together into multi-peak tracks, but compiles information  that should help to do that. It calculates
     several quantities related to the separation of the track start/end in time, frequency, and the difference between the slopes. The data
     class it constructs with these calculations is a more general class which compares a track to a Multi-Peak Track (MPT). In this case, the
     MPT simply has one track so it is an individual track-track comparison.

     Configuration name: "track-pair-comparer"

     Available configuration values:
     (none)

     Slots:
     - "track": void (shared_ptr<KTData>) -- If this is a new acquisition; Adds tracks to the internally-stored set of points; Requires KTProcessedTrackData.
     - "do-comparisons": void () -- Triggers calculations

     Signals:
     - "mt-comp": void (shared_ptr<KTData>) -- Emitted for each comparison; Guarantees KTMultiTrackComparison.
     - "comparisons-done": void () -- Emitted when calculations are complete
    */

    class KTTrackPairComparer : public Nymph::KTPrimaryProcessor
    {
        public:
            KTTrackPairComparer(const std::string& name = "track-pair-comparer");
            virtual ~KTTrackPairComparer();

            bool Configure(const scarab::param_node* node);

        public:
            // Store point information locally
            bool TakeTrack(KTProcessedTrackData& track);

            void SetNComponents(unsigned nComps);

            bool DoComparisons();

            bool Run();

        private:
            bool CompareTracks();

            std::vector< TrackSet > fCompTracks; // input tracks

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fMTCompSignal;
            Nymph::KTSignalOneArg< void > fDoneSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTProcessedTrackData > fTakeTrackSlot;

            void DoComparisonsSlot();

    };

}
 /* namespace Katydid */
#endif /* KTTRACKPAIRCOMPARER_HH_ */
