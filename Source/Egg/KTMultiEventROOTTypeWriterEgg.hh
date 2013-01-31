/*
 * KTBasicROOTTypeWriterEgg.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTMULTIEVENTROOTTYPEWRITEREGG_HH_
#define KTMULTIEVENTROOTTYPEWRITEREGG_HH_

#include "KTMultiEventROOTWriter.hh"

class TH1D;

namespace Katydid
{
    class KTEggHeader;
    class KTTimeSeriesData;

    class KTMultiEventROOTTypeWriterEgg : public KTMEROOTTypeWriterBase//, public KTTypeWriterEgg
    {
        public:
            friend class KTTimeSeriesData;

        public:
            KTMultiEventROOTTypeWriterEgg();
            virtual ~KTMultiEventROOTTypeWriterEgg();

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
#endif /* KTMULTIEVENTROOTTYPEWRITEREGG_HH_ */
