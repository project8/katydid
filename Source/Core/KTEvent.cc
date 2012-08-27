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
            fDataMap()
    {
    }

    KTEvent::~KTEvent()
    {
        for (DataMap::iterator it=fDataMap.begin(); it != fDataMap.end(); it++)
        {
            delete it->second;
        }
    }

    bool KTEvent::AddData(KTData* newData)
    {
        return AddData(newData->GetName(), newData);
    }

    bool KTEvent::AddData(const std::string& name, KTData* newData)
    {
        newData->fEvent = this;
        return (fDataMap.insert(DataMapVal(name, newData))).second;
    }





} /* namespace Katydid */
