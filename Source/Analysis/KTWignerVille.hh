/*
 * KTWignerVille.hh
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#ifndef KTWIGNERVILLE_HH_
#define KTWIGNERVILLE_HH_

#include "KTProcessor.hh"

namespace Katydid
{
    class KTComplexFFTW;
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumFFTW;
    class KTTimeSeriesData;
    class KTTimeSeriesFFTW;

    class KTWignerVille : public KTProcessor
    {
        public:
            typedef KTSignal< void (const KTWriteableData*) >::signal WVSignal;

        public:
            KTWignerVille();
            virtual ~KTWignerVille();

            Bool_t Configure(const KTPStoreNode* node);

            Bool_t GetSaveFrequencySpectrum() const;
            void SetSaveFrequencySpectrum(Bool_t flag);

        protected:
            KTComplexFFTW* fFFT;

            Bool_t fSaveFrequencySpectrum;

        public:
            /// Performs the W-V transform on the given time series data.
            /// In the process, the data is FFTed, and then reverse FFTed; if you want to keep the intermediate frequency spectrum, pass a KTFrequencySpectrumDataFFTW** as the second parameter..
            /// @note A frequency spectrum data object can still be returned even if the full W-V transform fails!
            KTTimeSeriesData* TransformData(const KTTimeSeriesData* data, KTFrequencySpectrumDataFFTW** outputFSData=NULL);

            /// Performs the W-V transform on the given time series.
            /// In the process, the data is FFTed, and then reverse FFTed. If you want to keep the intermediate frequency spectrum, pass a KTFrequencySpectrumFFTW** as the second parameter.
            /// @note A frequency spectrum object can still be returned even if the full W-V transform fails.
            KTTimeSeriesFFTW* Transform(const KTTimeSeriesFFTW* inputTS, KTFrequencySpectrumFFTW** outputFS=NULL);

            /// Performs the W-V transform on the given frequency spectrum (does NOT create a new FS)
            Bool_t Transform(KTFrequencySpectrumFFTW* freqSpectrum);

            //***************
             // Signals
             //***************

         private:
             WVSignal fWVSignal;

             //***************
             // Slots
             //***************

         public:
             //void ProcessHeader(const KTEggHeader* header);
             void ProcessEvent(KTEvent* event);
             void ProcessTimeSeriesData(const KTTimeSeriesData* tsData);
             void ProcessFrequencySpectrumData(const KTFrequencySpectrumDataFFTW* fsData);

    };

    inline Bool_t KTWignerVille::GetSaveFrequencySpectrum() const
    {
        return fSaveFrequencySpectrum;
    }

    inline void KTWignerVille::SetSaveFrequencySpectrum(Bool_t flag)
    {
        fSaveFrequencySpectrum = flag;
        return;
    }

} /* namespace Katydid */
#endif /* KTWIGNERVILLE_HH_ */
