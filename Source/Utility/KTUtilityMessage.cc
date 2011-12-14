#include "KTUtilityMessage.hh"
#include "KTMessageOrganizer.hh"

namespace Katydid
{

    KTUtilityMessage::KTUtilityMessage() :
        KTMessage()
    {
        fKey = string( "utility" );
        fSystemDescription = string( "UTILITY" );
        KTMessageOrganizer::GetInstance()->AddMessage( this );
    }
    KTUtilityMessage::~KTUtilityMessage()
    {
        KTMessageOrganizer::GetInstance()->RemoveMessage( this );
    }

    KTUtilityMessage& umsg = *((KTUtilityMessage*) (KTInitializer< KTUtilityMessage >::fData));

}
