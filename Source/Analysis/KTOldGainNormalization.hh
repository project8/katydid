/**
 @file KTOldGainNormalization.hh
 @brief Contains KTOldGainNormalization
 @details Removes slow variations in the baseline of a histogram
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTOLDGAINNORMALIZATION_HH_
#define KTOLDGAINNORMALIZATION_HH_

#include "KTProcessor.hh"

namespace Katydid
{
    class KTFrequencySpectrum;
    class KTPStoreNode;
    class KTSlidingWindowFSData;
    class KTPowerSpectrum;

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    class KTOldGainNormalization : public KTProcessor
    {
        public:
            KTOldGainNormalization();
            virtual ~KTOldGainNormalization();

            Bool_t Configure(const KTPStoreNode* node);

            void PrepareNormalization(KTFrequencySpectrum* fullArray, UInt_t reducedNBins, Double_t reducedBinWidth);

            void ProcessSlidingWindowFFT(KTSlidingWindowFSData* swFSData);
            void ProcessFrequencySpectrum(UInt_t psNum, KTFrequencySpectrum* powerSpectrum);

        private:
            KTFrequencySpectrum* fNormalization;

    };

} /* namespace Katydid */
#endif /* KTOLDGAINNORMALIZATION_HH_ */
