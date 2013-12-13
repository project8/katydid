/*
 * KTDataTypeDisplayFFT.hh
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#ifndef KTDATATYPEDISPLAYFFT_HH_
#define KTDATATYPEDISPLAYFFT_HH_

#include "KTDataDisplay.hh"

#include "KTData.hh"

namespace Katydid
{
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataFFTW;
    class KTMultiFSDataPolar;
    class KTMultiFSDataFFTW;

    class KTDataTypeDisplayFFT : public KTDataTypeDisplay
    {
        public:
            KTDataTypeDisplayFFT();
            virtual ~KTDataTypeDisplayFFT();

            void RegisterSlots();


            //************************
            // Frequency Spectrum Data
            //************************
        public:
            void DrawFrequencySpectrumDataPolar(KTDataPtr data);
            void DrawFrequencySpectrumDataFFTW(KTDataPtr data);
            void DrawFrequencySpectrumDataPolarPhase(KTDataPtr data);
            void DrawFrequencySpectrumDataFFTWPhase(KTDataPtr data);
            void DrawFrequencySpectrumDataPolarPower(KTDataPtr data);
            void DrawFrequencySpectrumDataFFTWPower(KTDataPtr data);

            void DrawFrequencySpectrumDataPolarMagnitudeDistribution(KTDataPtr data);
            void DrawFrequencySpectrumDataFFTWMagnitudeDistribution(KTDataPtr data);
            void DrawFrequencySpectrumDataPolarPowerDistribution(KTDataPtr data);
            void DrawFrequencySpectrumDataFFTWPowerDistribution(KTDataPtr data);

            //************************
            // Time/Frequency Data
            //************************
            void DrawTimeFrequencyDataPolar(KTDataPtr data);
            //void DrawFrequencySpectrumDataFFTW(KTDataPtr data);
            void DrawTimeFrequencyDataPolarPhase(KTDataPtr data);
            //void DrawFrequencySpectrumDataFFTWPhase(KTDataPtr data);
            void DrawTimeFrequencyDataPolarPower(KTDataPtr data);
            //void DrawFrequencySpectrumDataFFTWPower(KTDataPtr data);


            //************************
            // Multi-FS Data
            //************************
        public:
            void DrawMultiFSDataPolar(KTDataPtr data);
            void DrawMultiFSDataFFTW(KTDataPtr data);
    };

} /* namespace Katydid */
#endif /* KTDATATYPEDISPLAYFFT_HH_ */
