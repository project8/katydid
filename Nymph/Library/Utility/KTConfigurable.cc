/*
 * KTConfigurable.cc
 *
 *  Created on: Aug 21, 2012
 *      Author: nsoblath
 */

#include "KTConfigurable.hh"

#include "KTCommandLineHandler.hh"
#include "KTLogger.hh"
#include "KTConfigurator.hh"
#include "KTParam.hh"

using std::string;

namespace Nymph
{

    KTLOGGER(conflog, "KTConfigurable");

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

    bool KTSelfConfigurable::Configure()
    {
        if (fIsConfigured) return true;

        KTParamNode* node = KTConfigurator::GetInstance()->Config();
        if (! this->Configure(node))
        {
            KTERROR(conflog, "An error occurred while configuring <" << fConfigName << ">");
            return false;
        }
        fIsConfigured = IsReady();
        return fIsConfigured;
    }

} /* namespace Nymph */
