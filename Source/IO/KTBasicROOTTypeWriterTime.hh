/*
 * KTBasicROOTTypeWriterTime.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITERTIME_HH_
#define KTBASICROOTTYPEWRITERTIME_HH_

#include "KTBasicROOTFileWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    using namespace Nymph;
    class KTBasicROOTTypeWriterTime : public KTBasicROOTTypeWriter//, public KTTypeWriterTime
    {
        public:
            KTBasicROOTTypeWriterTime();
            virtual ~KTBasicROOTTypeWriterTime();

            void RegisterSlots();

        public:
            void WriteRawTimeSeriesData(KTDataPtr data);
            //void WriteRawTimeSeriesDataDistribution(KTDataPtr data);

            void WriteTimeSeriesData(KTDataPtr data);
            void WriteTimeSeriesDataDistribution(KTDataPtr data);

    };

} /* namespace Katydid */
#endif /* KTBASICROOTFILETYPEWRITERTIME_HH_ */
