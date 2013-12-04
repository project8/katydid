/*
 * KTBasicROOTTypeWriterFFT.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTISLICEROOTTYPEWRITERFFT_HH_
#define KTMULTISLICEROOTTYPEWRITERFFT_HH_

#include "KTMultiSliceROOTWriter.hh"

#include <boost/shared_ptr.hpp>

class TH1D;

namespace Katydid
{
    struct KTData;

    class KTMultiSliceROOTTypeWriterFFT : public KTMEROOTTypeWriterBase//, public KTTypeWriterFFT
    {
        public:
            KTMultiSliceROOTTypeWriterFFT();
            virtual ~KTMultiSliceROOTTypeWriterFFT();

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
#endif /* KTMULTISLICEROOTTYPEWRITERFFT_HH_ */
