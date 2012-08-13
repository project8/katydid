#ifndef KTCOREMESSAGE_H_
#define KTCOREMESSAGE_H_

#include "KTMessage.hh"
#include "KTInitializer.hh"

namespace Katydid
{

    class KTFrameworkMessage :
        public KTMessage
    {
        public:
            KTFrameworkMessage();
            virtual ~KTFrameworkMessage();
    };

    extern KTFrameworkMessage& fwmsg;
    static KTInitializer< KTFrameworkMessage > sfwmsg;
}

#endif // KTCOREMESSAGE_H_
