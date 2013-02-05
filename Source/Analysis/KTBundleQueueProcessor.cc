/*
 * KTBundleQueueProcessor.cc
 *
 *  Created on: Feb 5, 2013
 *      Author: nsoblath
 */

#include "KTBundleQueueProcessor.hh"

#include "KTPStoreNode.hh"

namespace Katydid
{
    static KTDerivedRegistrar< KTProcessor, KTBundleQueueProcessor > sSimpClustRegistrar("bundle-queue");

    KTBundleQueueProcessor::KTBundleQueueProcessor() :
        KTBundleQueueProcessorTemplate< KTBundleQueueProcessor >()
    {
        fConfigName = "bundle-queue";

        RegisterSlot("bundle", this, &KTBundleQueueProcessor::QueueBundle, "void (shared_ptr<KTBundle>)");
        RegisterSlot("bundle-list", this, &KTBundleQueueProcessor::QueueBundles, "void (list< shared_ptr<KTBundle> >)");
    }

    KTBundleQueueProcessor::~KTBundleQueueProcessor()
    {

    }

    Bool_t KTBundleQueueProcessor::Configure(const KTPStoreNode* node)
    {
        return true;
    }

    void KTBundleQueueProcessor::EmitBundleSignal()
    {

    }

}
