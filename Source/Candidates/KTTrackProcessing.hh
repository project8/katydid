/**
 @file KTTrackProcessing.hh
 @brief Contains KTTrackProcessing
 @details Extracts physics-relevant information about tracks
 @author: N.S. Oblath & B. LaRoque
 @date: July 22, 2013
 */

#ifndef KTTRACKPROCESSING_HH_
#define KTTRACKPROCESSING_HH_

#include "KTProcessor.hh"

#include "KTSlot.hh"



namespace Katydid
{
    class KTHoughData;
    class KTParamNode;
    class KTSparseWaterfallCandidateData;

    /*!
     @class KTTrackProcessing
     @author N.S. Oblath & B. LaRoque

     @brief Extracts physics-relevant information about tracks

     @details

     Configuration name: "track-proc"

     Available configuration values:
     - "some-name": [type] -- [what it does]

     Slots:
     - "swfc-and-hough": void (KTDataPr) -- [what it does]; Requires KTSparseWaterfallCandidateData and KTHoughData; Adds [output data type]; Emits signal "[signal-name]"

     Signals:
     - "track": void (KTDataPtr) -- Emitted when a track has been processed; Guarantees [output data type].
    */

    class KTTrackProcessing : public KTProcessor
    {
        public:
            KTTrackProcessing(const std::string& name = "track-proc");
            virtual ~KTTrackProcessing();

            bool Configure(const KTParamNode* node);

        private:

        public:
            bool ProcessTrack(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData);

        private:

            //***************
            // Signals
            //***************

        private:
            KTSignalData fTrackSignal;

            //***************
            // Slots
            //***************

        private:
            KTSlotDataTwoTypes< KTSparseWaterfallCandidateData, KTHoughData > fSWFAndHoughSlot;

    };
}
 /* namespace Katydid */
#endif /* KTTRACKPROCESSING_HH_ */
