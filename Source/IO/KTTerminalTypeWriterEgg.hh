/*
 * KTTerminalTypeWriterEgg.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTTERMINALTYPEWRITEREGG_HH_
#define KTTERMINALTYPEWRITEREGG_HH_

#include "KTTerminalWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    class KTTerminalTypeWriterEgg : public KTTerminalTypeWriter
    {
        public:
            KTTerminalTypeWriterEgg();
            virtual ~KTTerminalTypeWriterEgg();

            void RegisterSlots();

        public:
            void WriteTimeSeriesData(KTDataPtr data);
            void WriteDigitizerTestData(KTDataPtr data);
    };

} /* namespace Katydid */
#endif /* KTTERMINALTYPEWRITEREGG_HH_ */
