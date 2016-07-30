/**
 @file KTCandidateCleaner.hh
 @brief Contains KTCandidateCleaner
 @details Cleans candidates
 @author: N.S. Oblath & B. LaRoque
 @date: July 22, 2013
 */

#ifndef KTCANDIDATECLEANER_HH_
#define KTCANDIDATECLEANER_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"


namespace Katydid
{
    using namespace Nymph;
    class KTHoughData;
    class KTSparseWaterfallCandidateData;

    /*!
     @class KTCandidateCleaner
     @author N.S. Oblath & B. LaRoque

     @brief Cleans candidates

     @details

     Configuration name: "candidate-cleaner"

     Available configuration values:
     - "some-name": [type] -- [what it does]

     Slots:
     - "swfc-and-hough": void (KTDataPr) -- Removes points from the candidate that don't fall on the dominant line found by the Hough Transform ; Requires KTSparseWaterfallCandidateData and KTHoughData; Does not add data; Emits signal "clean-cand"

     Signals:
     - "clean-cand": void (KTDataPtr) -- Emitted when a candidate has been cleaned; Guarantees [output data type].
    */

    class KTCandidateCleaner : public KTProcessor
    {
        public:
            KTCandidateCleaner(const std::string& name = "candidate-cleaner");
            virtual ~KTCandidateCleaner();

            bool Configure(const scarab::param_node* node);

        private:

        public:
            bool CleanCandidate(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData);

        private:

            //***************
            // Signals
            //***************

        private:
            KTSignalData fCleanedCandidateSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTSparseWaterfallCandidateData, KTHoughData > fSWFCAndHoughSlot;

    };
}
 /* namespace Katydid */
#endif /* KTCANDIDATECLEANER_HH_ */
