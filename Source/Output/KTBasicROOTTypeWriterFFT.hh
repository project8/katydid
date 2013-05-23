/*
 * KTBasicROOTTypeWriterFFT.hh
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITERFFT_HH_
#define KTBASICROOTTYPEWRITERFFT_HH_

#include "KTBasicROOTFileWriter.hh"

namespace Katydid
{
    class KTData;
    class KTFrequencySpectrumDataPolar;
    class KTFrequencySpectrumDataFFTW;
    class KTMultiFSDataPolar;
    class KTMultiFSDataFFTW;

    class KTBasicROOTTypeWriterFFT : public KTBasicROOTTypeWriter
    {
        public:
            KTBasicROOTTypeWriterFFT();
            virtual ~KTBasicROOTTypeWriterFFT();

            void RegisterSlots();


            //************************
            // Frequency Spectrum Data
            //************************
        public:
            void WriteFrequencySpectrumDataPolar(boost::shared_ptr<KTData> data);
            void WriteFrequencySpectrumDataFFTW(boost::shared_ptr<KTData> data);
            void WriteFrequencySpectrumDataPolarPhase(boost::shared_ptr<KTData> data);
            void WriteFrequencySpectrumDataFFTWPhase(boost::shared_ptr<KTData> data);
            void WriteFrequencySpectrumDataPolarPower(boost::shared_ptr<KTData> data);
            void WriteFrequencySpectrumDataFFTWPower(boost::shared_ptr<KTData> data);

            void WriteFrequencySpectrumDataPolarMagnitudeDistribution(boost::shared_ptr<KTData> data);
            void WriteFrequencySpectrumDataFFTWMagnitudeDistribution(boost::shared_ptr<KTData> data);
            void WriteFrequencySpectrumDataPolarPowerDistribution(boost::shared_ptr<KTData> data);
            void WriteFrequencySpectrumDataFFTWPowerDistribution(boost::shared_ptr<KTData> data);

            //************************
            // Time/Frequency Data
            //************************
            void WriteTimeFrequencyDataPolar(boost::shared_ptr<KTData> data);
            //void WriteFrequencySpectrumDataFFTW(boost::shared_ptr<KTData> data);
            void WriteTimeFrequencyDataPolarPhase(boost::shared_ptr<KTData> data);
            //void WriteFrequencySpectrumDataFFTWPhase(boost::shared_ptr<KTData> data);
            void WriteTimeFrequencyDataPolarPower(boost::shared_ptr<KTData> data);
            //void WriteFrequencySpectrumDataFFTWPower(boost::shared_ptr<KTData> data);


            //************************
            // Multi-FS Data
            //************************
        public:
            void WriteMultiFSDataPolar(boost::shared_ptr<KTData> data);
            void WriteMultiFSDataFFTW(boost::shared_ptr<KTData> data);
    };

} /* namespace Katydid */
#endif /* KTBASICROOTTYPEWRITERFFT_HH_ */
