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
    
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataFFTW;
    class KTMultiFSDataPolar;
    class KTMultiFSDataFFTW;
    class KTPowerSpectrumData;

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
            void WriteFrequencySpectrumDataPolar(Nymph::KTDataPtr data);
            void WriteFrequencySpectrumDataFFTW(Nymph::KTDataPtr data);
            void WriteFrequencySpectrumDataPolarPhase(Nymph::KTDataPtr data);
            void WriteFrequencySpectrumDataFFTWPhase(Nymph::KTDataPtr data);
            void WriteFrequencySpectrumDataPolarPower(Nymph::KTDataPtr data);
            void WriteFrequencySpectrumDataFFTWPower(Nymph::KTDataPtr data);

            void WriteFrequencySpectrumDataPolarMagnitudeDistribution(Nymph::KTDataPtr data);
            void WriteFrequencySpectrumDataFFTWMagnitudeDistribution(Nymph::KTDataPtr data);
            void WriteFrequencySpectrumDataPolarPowerDistribution(Nymph::KTDataPtr data);
            void WriteFrequencySpectrumDataFFTWPowerDistribution(Nymph::KTDataPtr data);

            void WriteFrequencySpectrumVarianceDataPolar(Nymph::KTDataPtr data);
            void WriteFrequencySpectrumVarianceDataFFTW(Nymph::KTDataPtr data);


            //********************
            // Power Spectrum Data
            //********************
            void WritePowerSpectrum(Nymph::KTDataPtr data);
            void WritePowerSpectralDensity(Nymph::KTDataPtr data);

            void WritePowerSpectrumDistribution(Nymph::KTDataPtr data);
            void WritePowerSpectralDensityDistribution(Nymph::KTDataPtr data);

            void WritePowerSpectrumVarianceData(Nymph::KTDataPtr data);


            //************************
            // Time/Frequency Data
            //************************
            void WriteTimeFrequencyDataPolar(Nymph::KTDataPtr data);
            //void WriteFrequencySpectrumDataFFTW(Nymph::KTDataPtr data);
            void WriteTimeFrequencyDataPolarPhase(Nymph::KTDataPtr data);
            //void WriteFrequencySpectrumDataFFTWPhase(Nymph::KTDataPtr data);
            void WriteTimeFrequencyDataPolarPower(Nymph::KTDataPtr data);
            //void WriteFrequencySpectrumDataFFTWPower(Nymph::KTDataPtr data);


            //************************
            // Multi-FS Data
            //************************
        public:
            void WriteMultiFSDataPolar(Nymph::KTDataPtr data);
            void WriteMultiFSDataFFTW(Nymph::KTDataPtr data);
    };

} /* namespace Katydid */
#endif /* KTBASICROOTTYPEWRITERTRANSFORM_HH_ */
