/*
 * KTBasicROOTTypeWriterEgg.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTIBUNDLEROOTTYPEWRITEREGG_HH_
#define KTMULTIBUNDLEROOTTYPEWRITEREGG_HH_

#include "KTMultiBundleROOTWriter.hh"

#include <boost/shared_ptr.hpp>

class TH1D;

namespace Katydid
{
    class KTEggHeader;
    class KTData;

    class KTMultiBundleROOTTypeWriterEgg : public KTMEROOTTypeWriterBase//, public KTTypeWriterEgg
    {
        public:
            KTMultiBundleROOTTypeWriterEgg();
            virtual ~KTMultiBundleROOTTypeWriterEgg();

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
#endif /* KTMULTIBUNDLEROOTTYPEWRITEREGG_HH_ */
