/**
 @file KTExtensibleEvent.hh
 @brief Contains KTExtensibleEvent
 @details This extension of KTEvent allows you to attach other types of data (i.e. from a particular analysis)
 The added data objects are stored in an unordered map.
 @author: N. S. Oblath
 @date: August 24, 2012
 */

#ifndef KTEXTENSIBLEEVENT_HH_
#define KTEXTENSIBLEEVENT_HH_

#include "KTDecoratedEvent.hh"

#include <boost/unordered_map.hpp>

namespace Katydid
{
    class KTData;

    class KTExtensibleEvent : public KTDecoratedEvent
    {
        protected:
            typedef boost::unordered_map< std::string, KTData* > DataMap;
            typedef DataMap::value_type DataMapVal;

        public:
            KTExtensibleEvent(KTAbstractEvent* event);
            virtual ~KTExtensibleEvent();

        private:
            KTExtensibleEvent();

        public:
            KTData* GetData(const std::string& name) const;
            template< typename DerivedData >
            DerivedData* GetData(const std::string& name) const;

            /// Adds a data object to the extensible event; assumes ownership of the data.
            bool AddData(KTData* newData);

            bool RemoveData(const std::string& name);

        protected:
            DataMap fDataMap;

    };

    template< typename DerivedData >
    DerivedData* KTExtensibleEvent::GetData(const std::string& name) const
    {
        KTData* dataPtr = GetData(name);
        if (dataPtr == NULL) return NULL;
        return dynamic_cast< DerivedData* >(dataPtr);
    }

    inline KTData* KTExtensibleEvent::GetData(const std::string& name) const
    {
        try
        {
            return fDataMap.at(name);
        }
        catch (std::out_of_range& e)
        {
        }
        return NULL;
    }

    inline bool KTExtensibleEvent::AddData(KTData* newData)
    {
        return (fDataMap.insert(DataMapVal(newData->GetName(), newData))).second;
    }

    inline bool KTExtensibleEvent::RemoveData(const std::string& name)
    {
        return bool(fDataMap.erase(name) > 0);
    }

} /* namespace Katydid */

#endif /* KTEXTENSIBLEEVENT_HH_ */
