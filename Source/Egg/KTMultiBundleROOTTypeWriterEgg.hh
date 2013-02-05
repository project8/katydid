/*
 * KTBasicROOTTypeWriterEgg.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTIBUNDLEROOTTYPEWRITEREGG_HH_
#define KTMULTIBUNDLEROOTTYPEWRITEREGG_HH_

#include "KTMultiBundleROOTWriter.hh"

class TH1D;

namespace Katydid
{
    class KTEggHeader;
    class KTTimeSeriesData;

    class KTMultiBundleROOTTypeWriterEgg : public KTMEROOTTypeWriterBase//, public KTTypeWriterEgg
    {
        public:
            friend class KTTimeSeriesData;

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

            void AddTimeSeriesData(const KTTimeSeriesData* data);

        private:
            std::vector< TH1D* > fTSHists;

    };


} /* namespace Katydid */
#endif /* KTMULTIBUNDLEROOTTYPEWRITEREGG_HH_ */
