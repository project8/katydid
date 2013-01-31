/*
 * KTBasicROOTTypeWriterFFT.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTIEVENTROOTTYPEWRITERFFT_HH_
#define KTMULTIEVENTROOTTYPEWRITERFFT_HH_

#include "KTMultiEventROOTWriter.hh"

class TH1D;

namespace Katydid
{
    class KTFrequencySpectrumDataFFTW;
    class KTFrequencySpectrumData;

    class KTMultiEventROOTTypeWriterFFT : public KTMEROOTTypeWriterBase//, public KTTypeWriterFFT
    {
        public:
            friend class KTFrequencySpectrumData;

        public:
            KTMultiEventROOTTypeWriterFFT();
            virtual ~KTMultiEventROOTTypeWriterFFT();

            void RegisterSlots();

            virtual void StartNewHistograms();
            virtual void FinishHistograms();

            void OutputHistograms();

            void ClearHistograms();

        public:

            void AddFrequencySpectrumData(const KTFrequencySpectrumData* data);
            void AddFrequencySpectrumDataFFTW(const KTFrequencySpectrumDataFFTW* data);

        private:
            std::vector< TH1D* > fFSHists;
            std::vector< TH1D* > fFSFFTWHists;

    };


} /* namespace Katydid */
#endif /* KTMULTIEVENTROOTTYPEWRITERFFT_HH_ */
