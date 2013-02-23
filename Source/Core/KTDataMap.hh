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

#include <sstream>
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

            /// Returns a pointer to the data object, if it exists in the map
            virtual KTData* GetData(const std::string& name) = 0;

            /// Returns a pointer to the data object, if it exists in the map, and removes the object from the map
            virtual KTData* ExtractData(const std::string& name) = 0;

            /// Removes the object from the map and deletes it
            virtual Bool_t RemoveData(const std::string& name) = 0;

            /// Returns true if the map is empty
            virtual Bool_t Empty() = 0;

            /// Returns the number of data objects in the map
            virtual UInt_t Size() = 0;

            /// Prints the contents of the map
            virtual void PrintMap() = 0;
    };


    //*****************
    // KTDerivedDataMap
    //*****************

    template< class XDerivedData >
    class KTDerivedDataMap : public KTDataMap
    {
        public:
            typedef boost::unordered_map< std::string, XDerivedData* > DataMap;
            typedef XDerivedData MapType;
            typedef typename DataMap::value_type DataMapValue;
            typedef typename DataMap::iterator DataMapIt;
            typedef typename DataMap::const_iterator DataMapCIt;

        public:
            KTDerivedDataMap();
            virtual ~KTDerivedDataMap();

            Bool_t AddData(const std::string& name, XDerivedData* data);

            MapType* GetData(const std::string& name);

            MapType* ExtractData(const std::string& name);

            Bool_t RemoveData(const std::string& name);

            Bool_t Empty();

            UInt_t Size();

            void PrintMap();

            DataMapIt Begin();
            DataMapCIt Begin() const;
            DataMapCIt ConstBegin() const;

            DataMapIt End();
            DataMapCIt End() const;
            DataMapCIt ConstEnd() const;

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
    typename KTDerivedDataMap< XDerivedData >::MapType* KTDerivedDataMap< XDerivedData >::GetData(const std::string& name)
    {
        typename DataMap::const_iterator it = fDataMap.find(name);
        if (it == fDataMap.end()) return NULL;
        return it->second;
    }

    template< class XDerivedData >
    typename KTDerivedDataMap< XDerivedData >::MapType* KTDerivedDataMap< XDerivedData >::ExtractData(const std::string& name)
    {
        typename DataMap::const_iterator it = fDataMap.find(name);
        if (it == fDataMap.end()) return NULL;
        fDataMap.erase(name);
        return it->second;
    }

    template< class XDerivedData >
    Bool_t KTDerivedDataMap< XDerivedData >::RemoveData(const std::string& name)
    {
        typename DataMap::iterator it = fDataMap.find(name);
        if (it == fDataMap.end()) return true;
        delete it->second;
        return Bool_t(fDataMap.erase(name) > 0);
    }

    template< class XDerivedData >
    Bool_t KTDerivedDataMap< XDerivedData >::Empty()
    {
        return fDataMap.empty();
    }

    template< class XDerivedData >
    UInt_t KTDerivedDataMap< XDerivedData >::Size()
    {
        return (UInt_t)fDataMap.size();
    }

    template< class XDerivedData >
    void KTDerivedDataMap< XDerivedData >::PrintMap()
    {
        std::stringstream outStream;
        outStream << "Data objects:\n";
        for (DataMapCIt iter=fDataMap.cbegin(); iter != fDataMap.cend(); iter++)
        {
            outStream << '\t' << iter->second->GetName() << '\n';
        }
        KTDEBUG(corelog_data, outStream.str());
        return;
    }

    template< class XDerivedData >
    typename KTDerivedDataMap< XDerivedData >::DataMapIt KTDerivedDataMap< XDerivedData >::Begin()
    {
        return fDataMap.end();
    }
    template< class XDerivedData >
    typename KTDerivedDataMap< XDerivedData >::DataMapCIt KTDerivedDataMap< XDerivedData >::Begin() const
    {
        return fDataMap.cbegin();
    }
    template< class XDerivedData >
    typename KTDerivedDataMap< XDerivedData >::DataMapCIt KTDerivedDataMap< XDerivedData >::ConstBegin() const
    {
        return fDataMap.cbegin();
    }

    template< class XDerivedData >
    typename KTDerivedDataMap< XDerivedData >::DataMapIt KTDerivedDataMap< XDerivedData >::End()
    {
        return fDataMap.end();
    }
    template< class XDerivedData >
    typename KTDerivedDataMap< XDerivedData >::DataMapCIt KTDerivedDataMap< XDerivedData >::End() const
    {
        return fDataMap.cend();
    }
    template< class XDerivedData >
    typename KTDerivedDataMap< XDerivedData >::DataMapCIt KTDerivedDataMap< XDerivedData >::ConstEnd() const
    {
        return fDataMap.cend();
    }


} /* namespace Katydid */
#endif /* KTDATAMAP_HH_ */
