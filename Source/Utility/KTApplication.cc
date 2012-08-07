/*
 * KTApplication.cc
 *
 *  Created on: Aug 5, 2012
 *      Author: nsoblath
 */

#include "KTApplication.hh"

#include <boost/foreach.hpp>

#include <iostream>
using std::cout;
using std::endl;

using std::string;

namespace Katydid
{

    KTApplication::KTApplication(Bool_t makeTApp) :
            fCLHandler(KTCommandLineHandler::GetInstance()),
            fParamStore(KTParameterStore::GetInstance()),
            fConfigFilename(),
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

    void KTApplication::AddConfigOptionsToCLHandler(const KTParameterStore::PStoreTree* tree, const string& addressOfTree)
    {
        BOOST_FOREACH( KTParameterStore::PStoreTree::value_type const& v, tree->get_child("") )
        {
            KTParameterStore::PStoreTree subtree = v.second;
            string addressOfNode;
            if (addressOfTree.size() > 0) addressOfNode = addressOfTree + "." + v.first;
            else addressOfNode = v.first;

            if (tree->get< string >(v.first).length() > 0)
            {
                // Add this address to the CLHandler
                string helpMsg = "Configuration option: " + addressOfNode;
                fCLHandler->AddOption("Config File Options", helpMsg, addressOfNode);
            }

            // Recursively go down the hierarchy
            AddConfigOptionsToCLHandler(&subtree, addressOfNode);
        }
        return;
    }

    Bool_t KTApplication::ReadConfigFile()
    {
        if (! fParamStore->ReadConfigFile(fConfigFilename)) return false;
        AddConfigOptionsToCLHandler(fParamStore->GetTree());
        fCLHandler->FinalizeNewOptionGroups();
        if (fCLHandler->GetPrintHelpMessageAfterConfigFlag())
        {
            fCLHandler->PrintHelpMessageAndExit();
        }
        return true;
    }



} /* namespace Katydid */
