/*
 * KTBasicROOTTypeWriterTransform.hh
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITERTRANSFORM_HH_
#define KTBASICROOTTYPEWRITERTRANSFORM_HH_

#include "KTBasicROOTFileWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    using namespace Nymph;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataFFTW;
    class KTMultiFSDataPolar;
    class KTMultiFSDataFFTW;
    class KTPowerSpectrumData;
    class KTScoredSpectrumData;

    class KTBasicROOTTypeWriterTransform : public KTBasicROOTTypeWriter
    {
        public:
            KTBasicROOTTypeWriterTransform();
            virtual ~KTBasicROOTTypeWriterTransform();

            void RegisterSlots();


            //************************
            // Frequency Spectrum Data
            //************************
        public:
            void WriteFrequencySpectrumDataPolar(KTDataPtr data);
            void WriteFrequencySpectrumDataFFTW(KTDataPtr data);
            void WriteFrequencySpectrumDataPolarPhase(KTDataPtr data);
            void WriteFrequencySpectrumDataFFTWPhase(KTDataPtr data);
            void WriteFrequencySpectrumDataPolarPower(KTDataPtr data);
            void WriteFrequencySpectrumDataFFTWPower(KTDataPtr data);

            void WriteFrequencySpectrumDataPolarMagnitudeDistribution(KTDataPtr data);
            void WriteFrequencySpectrumDataFFTWMagnitudeDistribution(KTDataPtr data);
            void WriteFrequencySpectrumDataPolarPowerDistribution(KTDataPtr data);
            void WriteFrequencySpectrumDataFFTWPowerDistribution(KTDataPtr data);

            //********************
            // Power Spectrum Data
            //********************
            void WritePowerSpectrum(KTDataPtr data);
            void WritePowerSpectralDensity(KTDataPtr data);

            void WritePowerSpectrumDistribution(KTDataPtr data);
            void WritePowerSpectralDensityDistribution(KTDataPtr data);

            void WriteScoredSpectrum(KTDataPtr data);
            //************************
            // Time/Frequency Data
            //************************
            void WriteTimeFrequencyDataPolar(KTDataPtr data);
            //void WriteFrequencySpectrumDataFFTW(KTDataPtr data);
            void WriteTimeFrequencyDataPolarPhase(KTDataPtr data);
            //void WriteFrequencySpectrumDataFFTWPhase(KTDataPtr data);
            void WriteTimeFrequencyDataPolarPower(KTDataPtr data);
            //void WriteFrequencySpectrumDataFFTWPower(KTDataPtr data);


            //************************
            // Multi-FS Data
            //************************
        public:
            void WriteMultiFSDataPolar(KTDataPtr data);
            void WriteMultiFSDataFFTW(KTDataPtr data);
    };

} /* namespace Katydid */
#endif /* KTBASICROOTTYPEWRITERTRANSFORM_HH_ */
