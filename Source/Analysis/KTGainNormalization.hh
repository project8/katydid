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
    class KTFrequencySpectrum;
    class KTPStoreNode;
    class KTSlidingWindowFSData;
    class KTPowerSpectrum;

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    class KTGainNormalization : public KTProcessor, public KTConfigurable
    {
        public:
            KTGainNormalization();
            virtual ~KTGainNormalization();

            Bool_t Configure(const KTPStoreNode* node);

            void PrepareNormalization(KTFrequencySpectrum* fullArray, UInt_t reducedNBins, Double_t reducedBinWidth);

            void ProcessSlidingWindowFFT(KTSlidingWindowFSData* swFSData);
            void ProcessFrequencySpectrum(UInt_t psNum, KTFrequencySpectrum* powerSpectrum);

        private:
            KTFrequencySpectrum* fNormalization;

    };

} /* namespace Katydid */
#endif /* KTGAINNORMALIZATION_HH_ */
