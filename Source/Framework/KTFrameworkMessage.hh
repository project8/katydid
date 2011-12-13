#ifndef KTCOREMESSAGE_H_
#define KTCOREMESSAGE_H_

#include "KTMessage.hh"
#include "KTInitializer.hh"

namespace Katydid
{

    class KTCoreMessage :
        public KTMessage
    {
        public:
            KTCoreMessage();
            virtual ~KTCoreMessage();
    };

    extern KTCoreMessage& coremsg;
    static KTInitializer< KTCoreMessage > scoremsg;
}

#endif // KTCOREMESSAGE_H_
