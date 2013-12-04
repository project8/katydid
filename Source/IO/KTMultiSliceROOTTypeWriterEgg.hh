/*
 * KTBasicROOTTypeWriterEgg.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTISLICEROOTTYPEWRITEREGG_HH_
#define KTMULTISLICEROOTTYPEWRITEREGG_HH_

#include "KTMultiSliceROOTWriter.hh"

#include <boost/shared_ptr.hpp>

class TH1D;

namespace Katydid
{
    class KTEggHeader;
    struct KTData;

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
            void StartByHeader(const KTEggHeader* header);

            void AddTimeSeriesData(boost::shared_ptr<KTData> data);

        private:
            std::vector< TH1D* > fTSHists;

    };


} /* namespace Katydid */
#endif /* KTMULTISLICEROOTTYPEWRITEREGG_HH_ */
