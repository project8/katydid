/*
 * KTDataQueueProcessor.cc
 *
 *  Created on: Feb 5, 2013
 *      Author: nsoblath
 */

#include "KTDataQueueProcessor.hh"

#include "KTFactory.hh"
#include "KTPStoreNode.hh"

using boost::shared_ptr;

using std::list;

namespace Katydid
{
    static KTDerivedRegistrar< KTProcessor, KTDataQueueProcessor > sSimpClustRegistrar("data-queue");

    KTDataQueueProcessor::KTDataQueueProcessor() :
        KTDataQueueProcessorTemplate< KTDataQueueProcessor >()
    {
        fConfigName = "bundle-queue";

        RegisterSignal("bundle", &fDataSignal, "void (shared_ptr<KTData>)");

        RegisterSlot("data", this, &KTDataQueueProcessor::QueueData, "void (shared_ptr<KTData>&)");
        RegisterSlot("data-list", this, &KTDataQueueProcessor::QueueDataList, "void (list< shared_ptr<KTData>& >)");
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

    void KTDataQueueProcessor::QueueData(shared_ptr< KTData>& data)
    {
        return DoQueueData(data, &KTDataQueueProcessor::EmitDataSignal);
    }

    void KTDataQueueProcessor::QueueDataList(list< shared_ptr< KTData >& >* dataList)
    {
        return DoQueueDataList(dataList, &KTDataQueueProcessor::EmitDataSignal);
    }
}
