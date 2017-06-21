/**
 @file KTMultiPeakTrackBuilder.hh
 @brief Contains KTMultiPeakTrackBuilder
 @details Groups parallel tracks into multi-peak track data
 @author: E. Zayas
 @date: Dec 5, 2016
 */

#ifndef KTMULTIPEAKTRACKBUILDER_HH_
#define KTMULTIPEAKTRACKBUILDER_HH_

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
     @class KTMultiPeakTrackBuilder
     @author E. Zayas

     @brief Identifies and groups multi-peak tracks (i.e. tracks with sidebands)

     @details
     Groups parallel tracks into MPT structures by matching start/end timestamps within a tolerance.

     Configuration name: "multi-peak-track-builder"

     Available configuration values:
     - "sideband-time-tol": maximum difference in timestamps to treat as parallel tracks

     Slots:
     - "track": void (shared_ptr<KTData>) -- If this is a new acquisition; Adds tracks to the internally-stored set of points; Requires KTProcessedTrackData.
     - "do-clustering": void () -- Triggers clustering algorithm

     Signals:
     - "mpt": void (shared_ptr<KTData>) -- Emitted for each group found; Guarantees KTMultiPeakTrackData.
     - "clustering-done": void () -- Emitted when track clustering is complete
    */

    class KTMultiPeakTrackBuilder : public Nymph::KTPrimaryProcessor
    {
        public:
            KTMultiPeakTrackBuilder(const std::string& name = "multi-peak-track-builder");
            virtual ~KTMultiPeakTrackBuilder();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, SidebandTimeTolerance);

        public:
            // Store point information locally
            bool TakeTrack(KTProcessedTrackData& track);

            void SetNComponents(unsigned nComps);
            void SetTimeBinWidth(double bw);
            void SetFreqBinWidth(double bw);

            bool DoClustering();

            bool Run();

            unsigned GetDataCount() const;

        private:
            bool FindMultiPeakTracks();

            double fTimeBinWidth;
            double fFreqBinWidth;

            uint64_t fCurrentAcquisitionID;

            std::vector< TrackSet > fCompTracks; // input tracks
            std::vector< std::set< MultiPeakTrackRef, MTRComp > > fMPTracks;

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fMPTSignal;
            Nymph::KTSignalOneArg< void > fDoneSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTProcessedTrackData > fTakeTrackSlot;

            void DoClusteringSlot();

    };

    inline void KTMultiPeakTrackBuilder::SetTimeBinWidth(double bw)
    {
        fTimeBinWidth = bw;
        return;
    }
    inline void KTMultiPeakTrackBuilder::SetFreqBinWidth(double bw)
    {
        fFreqBinWidth = bw;
        return;
    }

}
 /* namespace Katydid */
#endif /* KTMULTIPEAKTRACKBUILDER_HH_ */
