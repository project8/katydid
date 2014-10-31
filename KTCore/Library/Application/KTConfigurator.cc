/*
 * KTConfigurator.cc
 *
 *  Created on: Nov 5, 2013
 *      Author: nsoblath
 */

#include "KTConfigurator.hh"

#include "KTLogger.hh"

using std::string;

namespace Katydid
{
    KTLOGGER( conflog, "KTConfigurator" );

    KTConfigurator::KTConfigurator() :
            fMasterConfig( new KTParamNode() ),
            fParamBuffer( NULL ),
            fStringBuffer()
    {
    }

    KTConfigurator::~KTConfigurator()
    {
        delete fMasterConfig;
    }

    void KTConfigurator::Merge(const KTParamNode& aNode)
    {
        fMasterConfig->Merge(aNode);
        return;
    }

    KTParamNode* KTConfigurator::Config()
    {
        return fMasterConfig;
    }

    const KTParamNode* KTConfigurator::Config() const
    {
        return fMasterConfig;
    }

} /* namespace Katydid */
