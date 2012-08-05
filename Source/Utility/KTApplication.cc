/*
 * KTApplication.cc
 *
 *  Created on: Aug 5, 2012
 *      Author: nsoblath
 */

#include "KTApplication.hh"

#include "KTCommandLineHandler.hh"
#include "KTParameterStore.hh"
#include "KTPStoreNode.hh"

using std::string;

namespace Katydid
{

    KTApplication::KTApplication() :
            fCLHandler(KTCommandLineHandler::GetInstance()),
            fParamStore(KTParameterStore::GetInstance())
    {
    }

    KTApplication::KTApplication(int argC, char** argV) :
            fCLHandler(KTCommandLineHandler::GetInstance()),
            fParamStore(KTParameterStore::GetInstance())
    {
    }

    KTApplication::~KTApplication()
    {
    }

    void KTApplication::ProcessCommandLine()
    {
        fCLHandler->ProcessCommandLine();
        return;
    }

    Bool_t KTApplication::ReadConfigFile(const string& filename)
    {
        return fParamStore->ReadConfigFile(filename);
    }

    KTPStoreNode* KTApplication::GetNode(const string address) const
    {
        return fParamStore->GetNode(address);
    }

    KTCommandLineHandler* KTApplication::GetCommandLineHandler() const
    {
        return fCLHandler;
    }

    KTParameterStore* KTApplication::GetParameterStore() const
    {
        return fParamStore;
    }


} /* namespace Katydid */
