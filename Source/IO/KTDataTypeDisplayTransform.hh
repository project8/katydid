/*
 * KTDataTypeDisplayTransform.hh
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#ifndef KTDATATYPEDISPLAYTRANSFORM_HH_
#define KTDATATYPEDISPLAYTRANSFORM_HH_

#include "KTDataDisplay.hh"

#include "KTData.hh"

namespace Katydid
{
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataFFTW;
    class KTMultiFSDataPolar;
    class KTMultiFSDataFFTW;

    class KTDataTypeDisplayTransform : public KTDataTypeDisplay
    {
        public:
            KTDataTypeDisplayTransform();
            virtual ~KTDataTypeDisplayTransform();

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
#endif /* KTDATATYPEDISPLAYTRANSFORM_HH_ */
