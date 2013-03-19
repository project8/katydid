/*
 * KTBasicROOTTypeWriterEgg.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTBASICROOTTYPEWRITEREGG_HH_
#define KTBASICROOTTYPEWRITEREGG_HH_

#include "KTBasicROOTFileWriter.hh"

#include <boost/shared_ptr.hpp>

namespace Katydid
{
    class KTData;

    class KTBasicROOTTypeWriterEgg : public KTBasicROOTTypeWriter//, public KTTypeWriterEgg
    {
        public:
            KTBasicROOTTypeWriterEgg();
            virtual ~KTBasicROOTTypeWriterEgg();

            void RegisterSlots();

        public:
            void WriteTimeSeriesData(boost::shared_ptr<KTData> data);

    };

} /* namespace Katydid */
#endif /* KTBASICROOTFILETYPEWRITER_HH_ */
