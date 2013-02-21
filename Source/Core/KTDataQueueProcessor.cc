/*
 * KTBundleQueueProcessor.cc
 *
 *  Created on: Feb 5, 2013
 *      Author: nsoblath
 */

#include "KTBundleQueueProcessor.hh"

#include "KTFactory.hh"
#include "KTPStoreNode.hh"

namespace Katydid
{
    static KTDerivedRegistrar< KTProcessor, KTBundleQueueProcessor > sSimpClustRegistrar("bundle-queue");

    KTBundleQueueProcessor::KTBundleQueueProcessor() :
        KTBundleQueueProcessorTemplate< KTBundleQueueProcessor >()
    {
        fConfigName = "bundle-queue";

        RegisterSignal("bundle", &fDataSignal, "void (shared_ptr<KTBundle>)");

        SetFuncPtr(&KTBundleQueueProcessor::EmitDataSignal);
    }

    KTBundleQueueProcessor::~KTBundleQueueProcessor()
    {

    }

    Bool_t KTBundleQueueProcessor::Configure(const KTPStoreNode* node)
    {
        return true;
    }

    void KTBundleQueueProcessor::EmitDataSignal(boost::shared_ptr<KTData> data)
    {
        fDataSignal(data);
        return;
    }
}
