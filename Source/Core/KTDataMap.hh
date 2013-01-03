/*
 * KTDataMap.hh
 *
 *  Created on: Jan 2, 2013
 *      Author: nsoblath
 */

#ifndef KTDATAMAP_HH_
#define KTDATAMAP_HH_

#include "KTLogger.hh"

#include "Rtypes.h"

#include <boost/unordered_map.hpp>

#include <string>

namespace Katydid
{
    KTLOGGER(corelog_data, "katydid.core");

    class KTData;

    //**********
    // KTDataMap
    //**********

    class KTDataMap
    {
        public:
            KTDataMap();
            virtual ~KTDataMap();

            virtual KTData* GetData(const std::string& name) = 0;

            virtual Bool_t RemoveData(const std::string& name) = 0;
    };


    //*****************
    // KTDerivedDataMap
    //*****************

    template< class XDerivedData >
    class KTDerivedDataMap : public KTDataMap
    {
        public:
            typedef boost::unordered_map< std::string, XDerivedData* > DataMap;

        public:
            KTDerivedDataMap();
            virtual ~KTDerivedDataMap();

            Bool_t AddData(const std::string& name, XDerivedData* data);

            KTData* GetData(const std::string& name);

            Bool_t RemoveData(const std::string& name);

        protected:
            DataMap fDataMap;
    };

    template< class XDerivedData >
    KTDerivedDataMap< XDerivedData >::KTDerivedDataMap() :
            KTDataMap(),
            fDataMap()
    {
    }

    template< class XDerivedData >
    KTDerivedDataMap< XDerivedData >::~KTDerivedDataMap()
    {
        for (typename DataMap::iterator it=fDataMap.begin(); it != fDataMap.end(); it++)
        {
            delete it->second;
        }
    }

    template< class XDerivedData >
    Bool_t KTDerivedDataMap< XDerivedData >::AddData(const std::string& name, XDerivedData* data)
    {
        if (fDataMap.find(name) != fDataMap.end())
        {
            KTERROR(corelog_data, "Unable to add data named <" << name << "> to map because data with that name already exists.");
            return false;
        }
        fDataMap[name] = data;
        return true;
    }

    template< class XDerivedData >
    KTData* KTDerivedDataMap< XDerivedData >::GetData(const std::string& name)
    {
        typename DataMap::const_iterator it = fDataMap.find(name);
        if (it == fDataMap.end()) return NULL;
        return it->second;
    }

    template< class XDerivedData >
    Bool_t KTDerivedDataMap< XDerivedData>::RemoveData(const std::string& name)
    {
        return Bool_t(fDataMap.erase(name) > 0);
    }

} /* namespace Katydid */
#endif /* KTDATAMAP_HH_ */
