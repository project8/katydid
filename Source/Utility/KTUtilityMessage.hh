#ifndef KTUTILITYMESSAGE_H_
#define KTUTILITYMESSAGE_H_

#include "KTMessage.hh"
#include "KTInitializer.hh"

namespace Katydid
{

    class KTUtilityMessage :
        public KTMessage
    {
        public:
            KTUtilityMessage();
            virtual ~KTUtilityMessage();
    };

    extern KTUtilityMessage& umsg;
    static KTInitializer< KTUtilityMessage > sumsg;
}

#endif // KTUTILITYMESSAGE_H_
