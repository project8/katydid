/*
 * KTBasicROOTTypeWriterEgg.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITEREGG_HH_
#define KTBASICROOTTYPEWRITEREGG_HH_

#include "KTBasicROOTFileWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    class KTBasicROOTTypeWriterEgg : public KTBasicROOTTypeWriter//, public KTTypeWriterEgg
    {
        public:
            KTBasicROOTTypeWriterEgg();
            virtual ~KTBasicROOTTypeWriterEgg();

            void RegisterSlots();

        public:
            void WriteRawTimeSeriesData(KTDataPtr data);
            //void WriteRawTimeSeriesDataDistribution(KTDataPtr data);

            void WriteTimeSeriesData(KTDataPtr data);
            void WriteTimeSeriesDataDistribution(KTDataPtr data);

    };

} /* namespace Katydid */
#endif /* KTBASICROOTFILETYPEWRITER_HH_ */
