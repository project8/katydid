/*
 * KTJSONTypeWriterTime.hh
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#ifndef KTJSONTYPEWRITERTIME_HH_
#define KTJSONTYPEWRITERTIME_HH_

#include "KTJSONWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    class KTEggHeader;

    class KTJSONTypeWriterTime : public KTJSONTypeWriter//, public KTTypeWriterTime
    {
        public:
            KTJSONTypeWriterTime();
            virtual ~KTJSONTypeWriterTime();

            void RegisterSlots();

        public:
            void WriteEggHeader(KTDataPtr headerPtr);
    };

} /* namespace Katydid */
#endif /* KTJSONTYPEWRITERTIME_HH_ */
