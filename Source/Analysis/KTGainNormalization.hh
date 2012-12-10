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

namespace Katydid
{
    class KTFrequencySpectrum;
    class KTFrequencySpectrumFFTW;
    class KTPStoreNode;
    //class KTSlidingWindowFSData;
    class KTSlidingWindowFSDataFFTW;
    class KTPowerSpectrum;

    template< size_t NDims, typename XDataType >
    class KTPhysicalArray;

    class KTGainNormalization : public KTProcessor
    {
        public:
            //typedef KTSignal< void (const KTSlidingWindowFSData*) >::signal SWFSSignal;
            typedef KTSignal< void (const KTSlidingWindowFSDataFFTW*) >::signal SWFSFFTWSignal;

        public:
            KTGainNormalization();
            virtual ~KTGainNormalization();

            Bool_t Configure(const KTPStoreNode* node);

            //void PrepareNormalization(KTFrequencySpectrum* fullArray, UInt_t reducedNBins, Double_t reducedBinWidth);
            void PrepareNormalizationFFTW(KTFrequencySpectrumFFTW* fullArray, UInt_t reducedNBins, Double_t reducedBinWidth);

            //void NormalizeData(KTSlidingWindowFSData* swFSData);
            //void NormalizeFrequencySpectrum(KTFrequencySpectrum* frequencySpectrum);
            void NormalizeFFTWData(KTSlidingWindowFSDataFFTW* swFSData);
            void NormalizeFFTWFrequencySpectrum(KTFrequencySpectrumFFTW* frequencySpectrum);

        private:
            KTFrequencySpectrum* fNormalization;

            //***************
            // Signals
            //***************

        private:
            //SWFSSignal fFSSignal
            SWFSFFTWSignal fFFTWFSSignal;

            //***************
            // Slots
            //***************

        public:
            void ProcessEvent(boost::shared_ptr<KTEvent> event);

    };

} /* namespace Katydid */
#endif /* KTGAINNORMALIZATION_HH_ */
