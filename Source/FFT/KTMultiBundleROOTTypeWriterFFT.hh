/*
 * KTBasicROOTTypeWriterFFT.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTIBUNDLEROOTTYPEWRITERFFT_HH_
#define KTMULTIBUNDLEROOTTYPEWRITERFFT_HH_

#include "KTMultiBundleROOTWriter.hh"

class TH1D;

namespace Katydid
{
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumDataPolar;

    class KTMultiBundleROOTTypeWriterFFT : public KTMEROOTTypeWriterBase//, public KTTypeWriterFFT
    {
        public:
            friend class KTFrequencySpectrumDataPolar;

        public:
            KTMultiBundleROOTTypeWriterFFT();
            virtual ~KTMultiBundleROOTTypeWriterFFT();

            void RegisterSlots();

            virtual void StartNewHistograms();
            virtual void FinishHistograms();

            void OutputHistograms();

            void ClearHistograms();

        public:

            void AddFrequencySpectrumData(const KTFrequencySpectrumDataPolar* data);
            void AddFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* data);

        private:
            std::vector< TH1D* > fFSHists;
            std::vector< TH1D* > fFSFFTWHists;

    };


} /* namespace Katydid */
#endif /* KTMULTIBUNDLEROOTTYPEWRITERFFT_HH_ */
