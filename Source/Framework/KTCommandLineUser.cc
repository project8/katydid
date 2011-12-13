/*
 * KTCommandLineUser.cxx
 *
 *  Created on: Nov 30, 2011
 *      Author: nsoblath
 */

#include "KTCommandLineUser.hh"

//#include "KTCommandLineHandler.hh"

namespace Katydid
{

    KTCommandLineUser::KTCommandLineUser() :
            fCLHandler(KTCommandLineHandler::GetInstance()),
            fCommandLineOptionsAdded(kFALSE),
            fCommandLineIsParsed(kFALSE)
    {
        fCLHandler->RegisterUser(this);
    }

    KTCommandLineUser::~KTCommandLineUser()
    {
        if (fCLHandler) fCLHandler->RemoveUser(this);
    }

} /* namespace Katydid */
