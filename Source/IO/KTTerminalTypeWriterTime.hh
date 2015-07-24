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
    class KTProcSummary;

    class KTTerminalTypeWriterTime : public KTTerminalTypeWriter
    {
        public:
            KTTerminalTypeWriterTime();
            virtual ~KTTerminalTypeWriterTime();

            void RegisterSlots();

        public:
            void WriteEggHeader(KTDataPtr data);

            void WriteTimeSeriesData(KTDataPtr data);
            void WriteDigitizerTestData(KTDataPtr data);

            void WriteProcSummary(const KTProcSummary* summary);
    };

} /* namespace Katydid */
#endif /* KTTERMINALTYPEWRITERTIME_HH_ */
