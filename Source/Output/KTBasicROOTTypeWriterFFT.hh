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
    //class KTSlidingWindowFSData;
    //class KTSlidingWindowFSDataFFTW;

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

            void WriteFrequencySpectrumDataPolarPowerDistribution(boost::shared_ptr<KTData> data);
            void WriteFrequencySpectrumDataFFTWPowerDistribution(boost::shared_ptr<KTData> data);

            //************************
            // Sliding Window Data
            //************************
/*
        public:
            void WriteSlidingWindowFSData(const KTSlidingWindowFSData* data);
            void WriteSlidingWindowFSDataFFTW(const KTSlidingWindowFSDataFFTW* data);
*/
    };

} /* namespace Katydid */
#endif /* KTBASICROOTTYPEWRITERFFT_HH_ */
