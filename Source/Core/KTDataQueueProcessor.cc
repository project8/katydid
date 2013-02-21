/*
 * KTDataQueueProcessor.cc
 *
 *  Created on: Feb 5, 2013
 *      Author: nsoblath
 */

#include "KTDataQueueProcessor.hh"

#include "KTFactory.hh"
#include "KTPStoreNode.hh"

namespace Katydid
{
    static KTDerivedRegistrar< KTProcessor, KTDataQueueProcessor > sSimpClustRegistrar("data-queue");

    KTDataQueueProcessor::KTDataQueueProcessor() :
        KTDataQueueProcessorTemplate< KTDataQueueProcessor >()
    {
        fConfigName = "bundle-queue";

        RegisterSignal("bundle", &fDataSignal, "void (shared_ptr<KTData>)");

        SetFuncPtr(&KTDataQueueProcessor::EmitDataSignal);
    }

    KTDataQueueProcessor::~KTDataQueueProcessor()
    {

    }

    Bool_t KTDataQueueProcessor::Configure(const KTPStoreNode* node)
    {
        return true;
    }

    void KTDataQueueProcessor::EmitDataSignal(boost::shared_ptr<KTData> data)
    {
        fDataSignal(data);
        return;
    }
}
