/*
 * KTBasicROOTTypeWriterTransform.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTISLICEROOTTYPEWRITERTRANSFORM_HH_
#define KTMULTISLICEROOTTYPEWRITERTRANSFORM_HH_

#include "KTMultiSliceROOTWriter.hh"

#include "KTData.hh"

class TH1D;

namespace Katydid
{
    using namespace Nymph;
    class KTMultiSliceROOTTypeWriterTransform : public KTMEROOTTypeWriterBase//, public KTTypeWriterTransform
    {
        public:
            KTMultiSliceROOTTypeWriterTransform();
            virtual ~KTMultiSliceROOTTypeWriterTransform();

            void RegisterSlots();

            virtual void StartNewHistograms();
            virtual void FinishHistograms();

            void OutputHistograms();

            void ClearHistograms();

        public:

            void AddFrequencySpectrumDataPolar(KTDataPtr data);
            void AddFrequencySpectrumDataFFTW(KTDataPtr data);

        private:
            std::vector< TH1D* > fFSHists;
            std::vector< TH1D* > fFSFFTWHists;

    };


} /* namespace Katydid */
#endif /* KTMULTISLICEROOTTYPEWRITERTRANSFORM_HH_ */
