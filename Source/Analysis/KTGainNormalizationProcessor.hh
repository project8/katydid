/**
 @file KTGainNormalizationProcessor.hh
 @brief Contains KTGainNormalizationProcessor
 @details Removes slow variations in the baseline of a histogram
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTGAINNORMALIZATIONPROCESSOR_HH_
#define KTGAINNORMALIZATIONPROCESSOR_HH_

#include "KTProcessor.hh"

namespace Katydid
{
    class KTPowerSpectrum;
    class KTPStoreNode;
    class KTSlidingWindowFFT;

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    class KTGainNormalizationProcessor : public KTProcessor
    {
        public:
            KTGainNormalizationProcessor();
            virtual ~KTGainNormalizationProcessor();

            Bool_t Configure(const KTPStoreNode* node);

            Bool_t ApplySetting(const KTSetting* setting);

            void PrepareNormalization(KTPhysicalArray< 1, Double_t >* fullArray, UInt_t reducedNBins, Double_t reducedBinWidth);

            void ProcessSlidingWindowFFT(KTSlidingWindowFFT* fft);
            void ProcessPowerSpectrum(UInt_t psNum, KTPowerSpectrum* powerSpectrum);

        private:
            KTPhysicalArray< 1, Double_t >* fNormalization;

    };

} /* namespace Katydid */
#endif /* KTGAINNORMALIZATIONPROCESSOR_HH_ */
