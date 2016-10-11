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

#include <algorithm>
#include <set>
#include <vector>
#include <list>


namespace Katydid
{

    /*!
     @class KTMultiPeakEventBuilder
     @author N.S. Oblath

     @brief Builds tracks into events; identifies multi-peak tracks (i.e. tracks with sidebands), and then groups into events.

     @details

     Configuration name: "multi-peak-event-builder"

     Available configuration values:
     - "sideband-time-tol": 
     - "jump-time-tol":

     Slots:
     - "track": void (shared_ptr<KTData>) -- If this is a new acquisition; Adds tracks to the internally-stored set of points; Requires KTSliceHeader and KTDiscriminatedPoints1DData.
     - "do-clustering": void () -- Triggers clustering algorithm

     Signals:
     - "event": void (shared_ptr<KTData>) -- Emitted for each cluster found; Guarantees KT???Data.
     - "clustering-done": void () -- Emitted when track clustering is complete
    */

    class KTMultiPeakEventBuilder : public Nymph::KTPrimaryProcessor
    {
        public:
            KTMultiPeakEventBuilder(const std::string& name = "multi-peak-event-builder");
            virtual ~KTMultiPeakEventBuilder();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, SidebandTimeTolerance);

            MEMBERVARIABLE(double, JumpTimeTolerance);

        public:
            // Store point information locally
            bool TakeTrack(KTProcessedTrackData& track);

            void SetNComponents(unsigned nComps);
            void SetTimeBinWidth(double bw);
            void SetFreqBinWidth(double bw);

            bool Run();

            bool DoClustering();

            const std::set< Nymph::KTDataPtr >& GetCandidates() const;
            unsigned GetDataCount() const;

        private:
            bool FindMultiPeakTracks();
            bool FindEvents();

            double fTimeBinWidth;
            double fFreqBinWidth;


            std::vector< TrackSet > fCompTracks; // input tracks
            std::vector< std::set< Katydid::MultiPeakTrackRef, MTRComp > > fMPTracks;

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
            Nymph::KTSlotDataOneType< KTProcessedTrackData > fTakeTrackSlot;

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
