/*
 * KTBasicROOTTypeWriterEgg.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTISLICEROOTTYPEWRITEREGG_HH_
#define KTMULTISLICEROOTTYPEWRITEREGG_HH_

#include "KTMultiSliceROOTWriter.hh"

#include "KTData.hh"

class TH1D;

namespace Katydid
{
    class KTEggHeader;

    class KTMultiSliceROOTTypeWriterEgg : public KTMEROOTTypeWriterBase//, public KTTypeWriterEgg
    {
        public:
            KTMultiSliceROOTTypeWriterEgg();
            virtual ~KTMultiSliceROOTTypeWriterEgg();

            void RegisterSlots();

            virtual void StartNewHistograms();
            virtual void FinishHistograms();

            void OutputHistograms();

            void ClearHistograms();

        public:
            void StartByHeader(KTDataPtr);

            void AddTimeSeriesData(KTDataPtr data);

        private:
            std::vector< TH1D* > fTSHists;

    };


} /* namespace Katydid */
#endif /* KTMULTISLICEROOTTYPEWRITEREGG_HH_ */
