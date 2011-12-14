#include "KTFrameworkMessage.hh"
#include "KTMessageOrganizer.hh"

namespace Katydid
{

    KTFrameworkMessage::KTFrameworkMessage() :
        KTMessage()
    {
        fKey = string( "framework" );
        fSystemDescription = string( "FRAMEWORK" );
        KTMessageOrganizer::GetInstance()->AddMessage( this );
    }
    KTFrameworkMessage::~KTFrameworkMessage()
    {
        KTMessageOrganizer::GetInstance()->RemoveMessage( this );
    }

    KTFrameworkMessage& fwmsg = *((KTFrameworkMessage*) (KTInitializer< KTFrameworkMessage >::fData));

}
