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

        RegisterSignal("bundle", &fBundleSignal, "void (shared_ptr<KTBundle>)");

        SetFuncPtr(&KTBundleQueueProcessor::EmitBundleSignal);
    }

    KTBundleQueueProcessor::~KTBundleQueueProcessor()
    {

    }

    Bool_t KTBundleQueueProcessor::Configure(const KTPStoreNode* node)
    {
        return true;
    }

    void KTBundleQueueProcessor::EmitBundleSignal(boost::shared_ptr<KTBundle> bundle)
    {
        fBundleSignal(bundle);
        return;
    }
}
