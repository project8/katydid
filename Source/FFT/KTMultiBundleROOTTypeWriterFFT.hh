/*
 * KTBasicROOTTypeWriterFFT.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTIBUNDLEROOTTYPEWRITERFFT_HH_
#define KTMULTIBUNDLEROOTTYPEWRITERFFT_HH_

#include "KTMultiBundleROOTWriter.hh"

#include <boost/shared_ptr.hpp>

class TH1D;

namespace Katydid
{
    class KTData;

    class KTMultiBundleROOTTypeWriterFFT : public KTMEROOTTypeWriterBase//, public KTTypeWriterFFT
    {
        public:
            KTMultiBundleROOTTypeWriterFFT();
            virtual ~KTMultiBundleROOTTypeWriterFFT();

            void RegisterSlots();

            virtual void StartNewHistograms();
            virtual void FinishHistograms();

            void OutputHistograms();

            void ClearHistograms();

        public:

            void AddFrequencySpectrumDataPolar(boost::shared_ptr<KTData> data);
            void AddFrequencySpectrumDataFFTW(boost::shared_ptr<KTData> data);

        private:
            std::vector< TH1D* > fFSHists;
            std::vector< TH1D* > fFSFFTWHists;

    };


} /* namespace Katydid */
#endif /* KTMULTIBUNDLEROOTTYPEWRITERFFT_HH_ */
