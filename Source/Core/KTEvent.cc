/*
 * KTEvent.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEvent.hh"

#include "KTData.hh"

namespace Katydid
{

    KTEvent::KTEvent() :
            fEventNum(0),
            fIsLastEvent(false),
            fDataMapFactory(KTTIFactory< KTDataMap >::GetInstance()),
            fMapOfDataMaps(),
            fDataNameMap()
    {
        // Use the data map factories to build the data maps
        KTTIFactory< KTDataMap >::FactoryCIt endFactory = fDataMapFactory->GetFactoryMapEnd();
        for (KTTIFactory< KTDataMap >::FactoryCIt factIt = fDataMapFactory->GetFactoryMapBegin(); factIt != endFactory; factIt++)
        {
            fMapOfDataMaps.insert(MapOfDataMaps::value_type(factIt->first, fDataMapFactory->Create(factIt)));
        }
    }

    KTEvent::~KTEvent()
    {
#ifdef KATYDID_DEBUG
        KTDEBUG(corelog_event, "Data inventory at time of event deletion:");
        PrintAttachedData();
#endif
        for (MapOfDataMaps::iterator it=fMapOfDataMaps.begin(); it != fMapOfDataMaps.end(); it++)
        {
            delete it->second;
        }
    }
    /*
    bool KTEvent::AddData(KTData* newData)
    {
        return AddData(newData->GetName(), newData);
    }

    bool KTEvent::AddData(const std::string& name, KTData* newData)
    {
        newData->fEvent = this;
        return (fDataMap.insert(DataMapVal(name, newData))).second;
    }
    */

    void KTEvent::PrintAttachedData() const
    {
        for (MapOfDataMaps::const_iterator modmIt = fMapOfDataMaps.begin(); modmIt != fMapOfDataMaps.end(); modmIt++)
        {
            if (! modmIt->second->Empty())
            {
                KTDEBUG(corelog_event, "Data type: " << modmIt->first->name());
                modmIt->second->PrintMap();
            }
        }
    }

} /* namespace Katydid */
