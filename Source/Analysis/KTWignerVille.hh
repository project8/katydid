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
            KTWignerVille();
            virtual ~KTWignerVille();

            Bool_t Configure(const KTPStoreNode* node);


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


    };

} /* namespace Katydid */
#endif /* KTWIGNERVILLE_HH_ */
