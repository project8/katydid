/**
 @file KTCandidateTrackFinder.hh
 @brief Contains KTCandidateTrackFinder
 @details Finds candidate tracks
 @author: N.S. Oblath & B. LaRoque
 @date: July 22, 2013
 */

#ifndef KTCANDIDATETRACKFINDER_HH_
#define KTCANDIDATETRACKFINDER_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"


namespace Katydid
{
    
    class KTHoughData;
    class KTSparseWaterfallCandidateData;

    /*!
     @class KTCandidateTrackFinder
     @author N.S. Oblath & B. LaRoque

     @brief Finds candidate tracks

     @details

     Configuration name: "candidate-track-finder"

     Available configuration values:
     - "some-name": [type] -- [what it does]

     Slots:
     - "swfc-and-hough": void (KTDataPr) -- Finds the track based on the dominant line from the Hough Transform ; Requires KTSparseWaterfallCandidateData and KTHoughData; Does not add data; Emits signal "clean-cand"

     Signals:
     - "cand-track": void (Nymph::KTDataPtr) -- Emitted when a track has been found; Guarantees [output data type].
    */

    class KTCandidateTrackFinder : public Nymph::KTProcessor
    {
        public:
            KTCandidateTrackFinder(const std::string& name = "candidate-track-finder");
            virtual ~KTCandidateTrackFinder();

            bool Configure(const scarab::param_node* node);

        private:

        public:
            bool FindTrack(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData);

        private:

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTrackSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataTwoTypes< KTSparseWaterfallCandidateData, KTHoughData > fSWFCAndHoughSlot;

    };
}
 /* namespace Katydid */
#endif /* KTCANDIDATETRACKFINDER_HH_ */
