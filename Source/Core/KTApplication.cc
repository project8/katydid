/*
 * KTApplication.cc
 *
 *  Created on: Aug 5, 2012
 *      Author: nsoblath
 */

#include "KTApplication.hh"

#include "KTConfigurable.hh"
#include "KTLogger.hh"

#include <boost/foreach.hpp>

#include <vector>
using std::vector;

using std::string;

#include <iostream>

namespace Katydid
{
    KTLOGGER(applog, "katydid.core");

    KTApplication::KTApplication(Bool_t makeTApp) :
            fCLHandler(KTCommandLineHandler::GetInstance()),
            fParamStore(KTParameterStore::GetInstance()),
            fConfigFilename()
    {

#ifdef ROOT_FOUND
        fTApp = NULL;
        if (makeTApp)
        {
            fTApp = new TApplication("", 0, 0);
        }
#else
        if (makeTApp)
        {
            KTWARN(applog, "TApplication requested, but Katydid has been built without ROOT dependence.");
        }
#endif
    }

    KTApplication::KTApplication(int argC, char** argV, Bool_t makeTApp) :
            fCLHandler(KTCommandLineHandler::GetInstance()),
            fParamStore(KTParameterStore::GetInstance())
    {
#ifdef ROOT_FOUND
        fTApp = NULL;
        if (makeTApp)
        {
            fTApp = new TApplication("", 0, 0);
        }
#else
        if (makeTApp)
        {
            KTWARN(applog, "TApplication requested, but Katydid has been built without ROOT dependence.");
        }
#endif
        fCLHandler->TakeArguments(argC, argV);
        fConfigFilename = fCLHandler->GetConfigFilename();
    }

    KTApplication::~KTApplication()
    {
#ifdef ROOT_FOUND
        delete fTApp;
#endif
    }

    void KTApplication::AddConfigOptionsToCLHandler(const KTParameterStore::PStoreTree* tree, const string& addressOfTree)
    {
        BOOST_FOREACH( const KTParameterStore::PStoreTree::value_type& treeNode, tree->get_child("") )
        {
            KTParameterStore::PStoreTree subtree = treeNode.second;
            string addressOfNode;
            if (addressOfTree.size() > 0) addressOfNode = addressOfTree + "." + treeNode.first;
            else addressOfNode = treeNode.first;

            if (tree->get< string >(treeNode.first).length() > 0)
            {
                // Add this address to the CLHandler
                string helpMsg = "Configuration option: " + addressOfNode;
                fCLHandler->AddOption< string >("Config File Options", helpMsg, addressOfNode, false);
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
        if (! FinishProcessingCommandLine())
        {
            KTERROR(applog, "Something went wrong while finishing processing of the command line arguments");
            return false;
        }
        return true;
    }

    void KTApplication::ApplyCLOptionsToParamStore(const po::parsed_options* parsedOpts)
    {
        BOOST_FOREACH( const vector< po::basic_option< char > >::value_type& anOption, parsedOpts->options )
        {
            // only use the first token for now, since arrays aren't yet implemented in the parameter store
            if (anOption.value.size() > 0)
            {
                if (fParamStore->NodeExists(anOption.string_key))
                {
                    fParamStore->ChangeValue(anOption.string_key, anOption.value.at(0));
                }
            }
        }
        return;
    }

    Bool_t KTApplication::FinishProcessingCommandLine()
    {
        if (! fCLHandler->DelayedCommandLineProcessing())
        {
            return false;
        }
        ApplyCLOptionsToParamStore(fCLHandler->GetParsedOptions());
        return true;
    }

    Bool_t KTApplication::Configure(KTConfigurable* toBeConfigured, const std::string& baseAddress)
    {
        string address = baseAddress;
        if (! address.empty())
        {
            address = address + ".";
        }
        address = address + toBeConfigured->GetConfigName();
        KTPStoreNode pStoreNode = GetNode(address);
        if (! pStoreNode.IsValid())
        {
            KTWARN(applog, "Did not find a PStoreNode at address <" << address << ">");
            return false;
        }
        return toBeConfigured->Configure(&pStoreNode);
    }

} /* namespace Katydid */
