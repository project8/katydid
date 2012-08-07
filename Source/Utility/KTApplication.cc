/*
 * KTApplication.cc
 *
 *  Created on: Aug 5, 2012
 *      Author: nsoblath
 */

#include "KTApplication.hh"

using std::string;

namespace Katydid
{

    KTApplication::KTApplication(Bool_t makeTApp) :
            fCLHandler(KTCommandLineHandler::GetInstance()),
            fParamStore(KTParameterStore::GetInstance()),
            fTApp(NULL)
    {
        if (makeTApp)
        {
            fTApp = new TApplication("", 0, 0);
        }
    }

    KTApplication::KTApplication(int argC, char** argV, Bool_t makeTApp) :
            fCLHandler(KTCommandLineHandler::GetInstance()),
            fParamStore(KTParameterStore::GetInstance()),
            fTApp(NULL)
    {
        if (makeTApp)
        {
            fTApp = new TApplication("", 0, 0);
        }
        fCLHandler->TakeArguments(argC, argV);
        fConfigFilename = fCLHandler->GetConfigFilename();
    }

    KTApplication::~KTApplication()
    {
        delete fTApp;
    }

} /* namespace Katydid */
