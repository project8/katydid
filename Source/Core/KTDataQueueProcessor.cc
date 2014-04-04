/*
 * KTDataQueueProcessor.cc
 *
 *  Created on: Feb 5, 2013
 *      Author: nsoblath
 */

#include "KTDataQueueProcessor.hh"

//using std::list;

namespace Katydid
{
    KT_REGISTER_PROCESSOR(KTDataQueueProcessor, "data-queue");

    KTDataQueueProcessor::KTDataQueueProcessor(const std::string& name) :
        KTDataQueueProcessorTemplate< KTDataQueueProcessor >(name),
        fDataSignal("data", this)
    {
        RegisterSlot("data", this, &KTDataQueueProcessor::QueueData);
        //RegisterSlot("data-list", this, &KTDataQueueProcessor::QueueDataList);
    }

    KTDataQueueProcessor::~KTDataQueueProcessor()
    {

    }

    bool KTDataQueueProcessor::ConfigureSubClass(const KTParamNode*)
    {
        return true;
    }

    void KTDataQueueProcessor::EmitDataSignal(KTDataPtr data)
    {
        fDataSignal(data);
        return;
    }

    void KTDataQueueProcessor::QueueData(KTDataPtr& data)
    {
        return DoQueueData(data, &KTDataQueueProcessor::EmitDataSignal);
    }
/*
    void KTDataQueueProcessor::QueueDataList(list< KTDataPtr >* dataList)
    {
        return DoQueueDataList(dataList, &KTDataQueueProcessor::EmitDataSignal);
    }
*/
}
