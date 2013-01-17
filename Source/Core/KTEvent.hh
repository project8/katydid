/**
 @file KTEvent.hh
 @brief Contains KTEvent
 @details An event represents the information recorded over a certain period of time.
 An event is extensible: it can include a variety of types of data (i.e. from a particular analysis)
 The added data objects are stored in an unordered map.
 @note: Prior to August 24, 2012, KTEvent was the current KTTimeSeriesData.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTEVENT_HH_
#define KTEVENT_HH_

#include "KTDataMap.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"

#include "Rtypes.h"

#include <boost/unordered_map.hpp>

namespace Katydid
{
    KTLOGGER(corelog_event, "katydid.core");

    class KTData;

    class KTEvent
    {
        protected:
            typedef boost::unordered_map< const std::type_info*, KTDataMap* > MapOfDataMaps;
            typedef boost::unordered_map< std::string, KTDataMap* > DataNameMap;

        public:
            KTEvent();
            virtual ~KTEvent();

            //*****************************
            // Intrinsic data
            //*****************************
        public:
            unsigned GetEventNumber() const;
            void SetEventNumber(unsigned num);

            Bool_t GetIsLastEvent() const;
            void SetIsLastEvent(Bool_t flag);

        protected:
            unsigned fEventNum;
            Bool_t fIsLastEvent;

            //*****************************
            // Extensible data
            //*****************************
        public:
            //KTData* GetData(const std::string& name) const;

            template< typename DerivedData >
            DerivedData* GetData(const std::string& name) const;

            /** @brief Adds a data object to the extensible event
             *  @details
             *    - Uses KTData::GetName() to extract the name for the key
             *    - Assumes ownership of the data.
             */
            template< typename DerivedData >
            Bool_t AddData(DerivedData* newData);

            /** @brief Adds a data object to the extensible event
             *  @details
             *    - Takes a key value as a parameter for custom key selection.
             *    - Assumes ownership of the data.
             */
            template< typename DerivedData >
            Bool_t AddData(const std::string& name, DerivedData* newData);

            Bool_t RemoveData(const std::string& name);

            void PrintAttachedData() const;

        protected:
            KTTIFactory< KTDataMap >* fDataMapFactory; // singleton; not owned by KTEvent

            MapOfDataMaps fMapOfDataMaps;

            DataNameMap fDataNameMap;

    };


    inline unsigned KTEvent::GetEventNumber() const
    {
        return fEventNum;
    }

    inline void KTEvent::SetEventNumber(unsigned num)
    {
        fEventNum = num;
        return;
    }

    inline Bool_t KTEvent::GetIsLastEvent() const
    {
        return fIsLastEvent;
    }

    inline void KTEvent::SetIsLastEvent(Bool_t flag)
    {
        fIsLastEvent = flag;
        return;
    }

    template< typename DerivedData >
    DerivedData* KTEvent::GetData(const std::string& name) const
    {
        MapOfDataMaps::const_iterator modmIt = fMapOfDataMaps.find(&typeid(KTDerivedDataMap< DerivedData >));
        if (modmIt == fMapOfDataMaps.end())
        {
            KTERROR(corelog_event, "Attempt to retrieve data called <" << name << "> failed because there is no data class registered under map type <" << typeid(KTDerivedDataMap< DerivedData >).name() << ">");
            return NULL;
        }

        KTData* dataPtr = modmIt->second->GetData(name);
        if (dataPtr == NULL)
        {
            //KTERROR(corelog_event, "Unable to find data called <" << name << "> with type <" << typeid(DerivedData).name() << ">");
            return NULL;
        }
        return static_cast< DerivedData* >(dataPtr);
    }

    template< typename DerivedData >
    Bool_t KTEvent::AddData(DerivedData* newData)
    {
        return AddData(newData->GetName(), newData);
    }

    template< typename DerivedData >
    Bool_t KTEvent::AddData(const std::string& name, DerivedData* newData)
    {
        if (fDataNameMap.find(name) != fDataNameMap.end())
        {
            KTERROR(corelog_event, "There is already a data object named <" << name << "> attached to this event.");
            return false;
        }

        MapOfDataMaps::const_iterator modmIt = fMapOfDataMaps.find(&typeid(KTDerivedDataMap< DerivedData >));
        if (modmIt == fMapOfDataMaps.end())
        {
            KTERROR(corelog_event, "Attempt to add data called <" << name << "> failed because there is no data class registered under map type <" << typeid(KTDerivedDataMap< DerivedData >).name() << ">");
            return false;
        }
        // If the data map has been found, we can reasonably assume its derived type will matched the derived data type.

        KTDerivedDataMap< DerivedData >* ddm = static_cast< KTDerivedDataMap< DerivedData >* >(modmIt->second);
        if (! ddm->AddData(name, newData))
        {
            KTERROR(corelog_event, "Attempt to add data called <" << name << "> to data map failed.");
            return false;
        }

        newData->fEvent = this;
        return true;
    }


    inline Bool_t KTEvent::RemoveData(const std::string& name)
    {
        DataNameMap::const_iterator nmIt = fDataNameMap.find(name);
        if (nmIt == fDataNameMap.end()) return false;

        fDataNameMap.erase(name);
        if (! nmIt->second->RemoveData(name))
        {
            KTWARN(corelog_event, "While removing data called <" << name << ">, data was not found in the data map");
        }
        return true;
    }

} /* namespace Katydid */

#endif /* KTEVENT_HH_ */
