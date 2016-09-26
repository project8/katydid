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
            void DrawFrequencySpectrumDataPolar(Nymph::KTDataPtr data);
            void DrawFrequencySpectrumDataFFTW(Nymph::KTDataPtr data);
            void DrawFrequencySpectrumDataPolarPhase(Nymph::KTDataPtr data);
            void DrawFrequencySpectrumDataFFTWPhase(Nymph::KTDataPtr data);
            void DrawFrequencySpectrumDataPolarPower(Nymph::KTDataPtr data);
            void DrawFrequencySpectrumDataFFTWPower(Nymph::KTDataPtr data);

            void DrawFrequencySpectrumDataPolarMagnitudeDistribution(Nymph::KTDataPtr data);
            void DrawFrequencySpectrumDataFFTWMagnitudeDistribution(Nymph::KTDataPtr data);
            void DrawFrequencySpectrumDataPolarPowerDistribution(Nymph::KTDataPtr data);
            void DrawFrequencySpectrumDataFFTWPowerDistribution(Nymph::KTDataPtr data);

            //************************
            // Time/Frequency Data
            //************************
            void DrawTimeFrequencyDataPolar(Nymph::KTDataPtr data);
            //void DrawFrequencySpectrumDataFFTW(Nymph::KTDataPtr data);
            void DrawTimeFrequencyDataPolarPhase(Nymph::KTDataPtr data);
            //void DrawFrequencySpectrumDataFFTWPhase(Nymph::KTDataPtr data);
            void DrawTimeFrequencyDataPolarPower(Nymph::KTDataPtr data);
            //void DrawFrequencySpectrumDataFFTWPower(Nymph::KTDataPtr data);


            //************************
            // Multi-FS Data
            //************************
        public:
            void DrawMultiFSDataPolar(Nymph::KTDataPtr data);
            void DrawMultiFSDataFFTW(Nymph::KTDataPtr data);
    };

} /* namespace Katydid */
#endif /* KTDATATYPEDISPLAYTRANSFORM_HH_ */
