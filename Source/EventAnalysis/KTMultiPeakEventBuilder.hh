/**
 @file KTMultiPeakEventBuilder.hh
 @brief Contains KTMultiPeakEventBuilder
 @details Groups tracks into events
 @author: N.S. Oblath
 @date: Dec 7, 2015
 */

#ifndef KTMULTIPEAKEVENTBUILDER_HH_
#define KTMULTIPEAKEVENTBUILDER_HH_

#include "KTPrimaryProcessor.hh"

#include "KTDBScan.hh"
#include "KTDistanceMatrix.hh"
#include "KTSlot.hh"
#include "KTData.hh"
#include "KTMemberVariable.hh"
#include "KTMultiTrackEventData.hh"
//#include "KTMultiPeakTrackData.hh"

#include <algorithm>
#include <set>
#include <vector>
#include <list>


namespace Katydid
{
    
    /*!
     @class KTMultiPeakEventBuilder
     @author N.S. Oblath

     @brief Builds multi-peak tracks into events.

     @details
     Groups MPT structures into MPT events by matching head/tail timestamps within a tolerance.

     Configuration name: "multi-peak-event-builder"

     Available configuration values:
     - "jump-time-tol": double -- maximum difference in time between two MPT structures to combine into an event

     Slots:
     - "mpt": void (shared_ptr<KTData>) -- If this is a new acquisition; Adds group of tracks to the internally-stored set of points; Requires KTMultiPeakTrackData; Adds nothing
     - "do-clustering": void () -- Triggers clustering algorithm

     Signals:
     - "event": void (shared_ptr<KTData>) -- Emitted for each cluster found; Guarantees KTMultiTrackEventData.
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


            std::vector< std::set< MultiPeakTrackRef, MTRComp > > fMPTracks;

            std::set< Nymph::KTDataPtr > fCandidates;
            unsigned fDataCount;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fEventSignal;
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
