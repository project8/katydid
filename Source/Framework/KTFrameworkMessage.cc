#include "KTCoreMessage.hh"
#include "KTMessageOrganizer.hh"

namespace Katydid
{

    KTCoreMessage::KTCoreMessage() :
        KTMessage()
    {
        fKey = string( "core" );
        fSystemDescription = string( "CORE" );
        KTMessageOrganizer::GetInstance()->AddMessage( this );
    }
    KTCoreMessage::~KTCoreMessage()
    {
        KTMessageOrganizer::GetInstance()->RemoveMessage( this );
    }

    KTCoreMessage& coremsg = *((KTCoreMessage*) (KTInitializer< KTCoreMessage >::fData));

}
