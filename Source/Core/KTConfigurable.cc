/*
 * KTConfigurable.cc
 *
 *  Created on: Aug 21, 2012
 *      Author: nsoblath
 */

#include "KTConfigurable.hh"

#include "KTCommandLineHandler.hh"
#include "KTLogger.hh"
#include "KTParameterStore.hh"
#include "KTPStoreNode.hh"

using std::string;

namespace Katydid
{

    KTLOGGER(conflog, "katydid.core");

    //******************
    // KTConfigurable
    //******************

    KTConfigurable::KTConfigurable(const string& name) :
            fCLHandler(KTCommandLineHandler::GetInstance()),
            fConfigName(name)
    {
    }

    KTConfigurable::~KTConfigurable()
    {
    }



    //**********************
    // KTSelfConfigurable
    //**********************

    KTSelfConfigurable::KTSelfConfigurable(const string& name) :
            KTConfigurable(name),
            fIsConfigured(false)
    {
    }

    KTSelfConfigurable::~KTSelfConfigurable()
    {
    }

    Bool_t KTSelfConfigurable::Configure()
    {
        if (fIsConfigured) return true;

        KTPStoreNode node = KTParameterStore::GetInstance()->GetNode(fConfigName);
        if (node.IsValid())
        {
            if (! this->Configure(&node))
            {
                KTERROR(conflog, "An error occurred while configuring <" << fConfigName << ">");
                return false;
            }
            fIsConfigured = IsReady();
        }
        return fIsConfigured;
    }

} /* namespace Katydid */
