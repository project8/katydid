/*
 * KTBasicROOTTypeWriterEgg.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITEREGG_HH_
#define KTBASICROOTTYPEWRITEREGG_HH_

#include "KTBasicROOTFileWriter.hh"

namespace Katydid
{
    class KTTimeSeriesData;

    class KTBasicROOTTypeWriterEgg : public KTBasicROOTTypeWriter
    {
        public:
            KTBasicROOTTypeWriterEgg();
            virtual ~KTBasicROOTTypeWriterEgg();

            void RegisterSlots();

        public:
            void WriteTimeSeriesData(const KTTimeSeriesData* data);

    };

} /* namespace Katydid */
#endif /* KTBASICROOTFILETYPEWRITER_HH_ */
