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
            void WriteEggHeader(Nymph::KTDataPtr data);

            void WriteTimeSeriesData(Nymph::KTDataPtr data);
            void WriteDigitizerTestData(Nymph::KTDataPtr data);

            void WriteProcSummary(const KTProcSummary* summary);
    };

} /* namespace Katydid */
#endif /* KTTERMINALTYPEWRITERTIME_HH_ */
