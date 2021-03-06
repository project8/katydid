/**
 @file KTMultiPeakEventBuilder.hh
 @brief Contains KTMultiPeakEventBuilder
 @details Groups tracks into events
 @author: N.S. Oblath, B.H. LaRoque
 @date: Dec 7, 2015
 */

#ifndef KTMULTIPEAKEVENTBUILDER_HH_
#define KTMULTIPEAKEVENTBUILDER_HH_

#include "KTPrimaryProcessor.hh"

#include "KTDistanceMatrix.hh"
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
     @class KTMultiPeakEventBuilder
     @author N.S. Oblath, B.H. LaRoque

     @brief Builds multi-peak tracks into events.

     @details
     Iterates over multi-peak track objects, each with a pre-determined value for the start and stop in both frequency and in time.
     Tracks which have both a start time and end time in common (within "sideband-time-tol") are grouped into a multi-peak object.
     Tracks which share a start or stop time, but not both, are also grouped, and the unknown topology attribute is set to true,
     indicating that reconstruction is suspect.
     All input tracks are grouped into a multi-peak object (some of which may only contain a single line).
     Multi-peak tracks are then grouped into events, where two tracks are in the same event if the start of one is within jump-time-tol of the other.

     Configuration name: "multi-peak-event-builder"

     Available configuration values:
     - "sideband-time-tol": double -- For an existing multi-peak track, a new track has the same start/end if it starts/ends within sideband-time-tol of the existing object.
        units match the units of start time and end time of the input track object, should be seconds
     - "jump-time-tol": double -- Given two multi-peak track objects, if the start of the second is within jump-time-tol of the first, they are grouped into an event.
        units match the units of start time and end time of the input track object, should be seconds

     Slots:
     - "mpt": void (KTDataPtr) -- If this is a new acquisition; Adds group of tracks to the internally-stored set of points; Requires KTMultiPeakTrackData; Adds nothing
     - "do-clustering": void () -- Triggers clustering algorithm

     Signals:
     - "event": void (KTDataPtr) -- Emitted for each event (set of multi-peak tracks) found; Guarantees KTMultiTrackEventData.
     - "mpt": void (KTDataPtr) -- Emitted for each MPT within an event; Guarantees KTMultiPeakTrackData. Note: fUnknownEventTopology from the original MPT is not preserved in this signal; each MPT will have the same value of fUnknownEventTopology as the multi-peak event.
     - "clustering-done": void () -- Emitted when track clustering is complete
    */

    class KTMultiPeakEventBuilder : public Nymph::KTPrimaryProcessor
    {
        public:
            KTMultiPeakEventBuilder(const std::string& name = "multi-peak-event-builder");
            virtual ~KTMultiPeakEventBuilder();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, JumpTimeTolerance);

        public:
            // Store point information locally
            bool TakeMPT(KTMultiPeakTrackData& mpt);

            void SetNComponents(unsigned nComps);
            void SetTimeBinWidth(double bw);
            void SetFreqBinWidth(double bw);

            bool Run();

            bool DoClustering();

            const std::set< Nymph::KTDataPtr >& GetCandidates() const;
            unsigned GetDataCount() const;

        private:
            bool FindEvents();

            double fTimeBinWidth;
            double fFreqBinWidth;

            uint64_t fCurrentAcquisitionID;

            std::vector< std::set< MultiPeakTrackRef, MTRComp > > fMPTracks;

            std::set< Nymph::KTDataPtr > fCandidates;
            unsigned fDataCount;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fEventSignal;
            Nymph::KTSignalData fMPTSignal;
            Nymph::KTSignalOneArg< void > fEventsDoneSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTMultiPeakTrackData > fTakeMPTSlot;

            void DoClusteringSlot();

    };

    inline void KTMultiPeakEventBuilder::SetTimeBinWidth(double bw)
    {
        fTimeBinWidth = bw;
        return;
    }
    inline void KTMultiPeakEventBuilder::SetFreqBinWidth(double bw)
    {
        fFreqBinWidth = bw;
        return;
    }

    inline const std::set< Nymph::KTDataPtr >& KTMultiPeakEventBuilder::GetCandidates() const
    {
        return fCandidates;
    }
    inline unsigned KTMultiPeakEventBuilder::GetDataCount() const
    {
        return fDataCount;
    }


}
 /* namespace Katydid */
#endif /* KTMULTIPEAKEVENTBUILDER_HH_ */
