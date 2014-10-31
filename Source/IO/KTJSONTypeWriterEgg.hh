/*
 * KTJSONTypeWriterEgg.hh
 *
 *  Created on: Jan 4, 2013
 *      Author: nsoblath
 */

#ifndef KTJSONTYPEWRITEREGG_HH_
#define KTJSONTYPEWRITEREGG_HH_

#include "KTJSONWriter.hh"

#include "KTData.hh"

namespace Katydid
{
    class KTEggHeader;

    class KTJSONTypeWriterEgg : public KTJSONTypeWriter//, public KTTypeWriterEgg
    {
        public:
            KTJSONTypeWriterEgg();
            virtual ~KTJSONTypeWriterEgg();

            void RegisterSlots();

        public:
            void WriteEggHeader(KTDataPtr headerPtr);
    };

} /* namespace Katydid */
#endif /* KTJSONTYPEWRITEREGG_HH_ */
