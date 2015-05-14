/*
 * KTTerminalTypeWriterTime.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTTERMINALTYPEWRITERTIME_HH_
#define KTTERMINALTYPEWRITERTIME_HH_

#include "KTTerminalWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    class KTTerminalTypeWriterTime : public KTTerminalTypeWriter
    {
        public:
            KTTerminalTypeWriterTime();
            virtual ~KTTerminalTypeWriterTime();

            void RegisterSlots();

        public:
            void WriteTimeSeriesData(KTDataPtr data);
            void WriteDigitizerTestData(KTDataPtr data);
    };

} /* namespace Katydid */
#endif /* KTTERMINALTYPEWRITERTIME_HH_ */
