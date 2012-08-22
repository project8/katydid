/**
 @file KTGainNormalization.hh
 @brief Contains KTGainNormalization
 @details Removes slow variations in the baseline of a histogram
 @author: N. S. Oblath
 @date: Jan 24, 2012
 */

#ifndef KTGAINNORMALIZATION_HH_
#define KTGAINNORMALIZATION_HH_

#include "KTProcessor.hh"
#include "KTConfigurable.hh"

namespace Katydid
{
    class KTPowerSpectrum;
    class KTPStoreNode;
    class KTSlidingWindowFFT;

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    class KTGainNormalization : public KTProcessor, public KTConfigurable
    {
        public:
            KTGainNormalization();
            virtual ~KTGainNormalization();

            Bool_t Configure(const KTPStoreNode* node);

            void PrepareNormalization(KTPhysicalArray< 1, Double_t >* fullArray, UInt_t reducedNBins, Double_t reducedBinWidth);

            void ProcessSlidingWindowFFT(KTSlidingWindowFFT* fft);
            void ProcessPowerSpectrum(UInt_t psNum, KTPowerSpectrum* powerSpectrum);

        private:
            KTPhysicalArray< 1, Double_t >* fNormalization;

    };

} /* namespace Katydid */
#endif /* KTGAINNORMALIZATION_HH_ */
