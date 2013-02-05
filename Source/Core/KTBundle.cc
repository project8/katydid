/*
 * KTBundle.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTBundle.hh"

#include "KTData.hh"

namespace Katydid
{

    KTBundle::KTBundle() :
            fBundleNum(0),
            fIsLastBundle(false),
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

    KTBundle::~KTBundle()
    {
#ifdef KATYDID_DEBUG
        KTDEBUG(corelog_bundle, "Data inventory at time of bundle deletion:");
        PrintAttachedData();
#endif
        for (MapOfDataMaps::iterator it=fMapOfDataMaps.begin(); it != fMapOfDataMaps.end(); it++)
        {
            delete it->second;
        }
    }
    /*
    bool KTBundle::AddData(KTData* newData)
    {
        return AddData(newData->GetName(), newData);
    }

    bool KTBundle::AddData(const std::string& name, KTData* newData)
    {
        newData->fBundle = this;
        return (fDataMap.insert(DataMapVal(name, newData))).second;
    }
    */

    void KTBundle::PrintAttachedData() const
    {
        for (MapOfDataMaps::const_iterator modmIt = fMapOfDataMaps.begin(); modmIt != fMapOfDataMaps.end(); modmIt++)
        {
            if (! modmIt->second->Empty())
            {
                KTDEBUG(corelog_bundle, "Data type: " << modmIt->first->name());
                modmIt->second->PrintMap();
            }
        }
    }

} /* namespace Katydid */
