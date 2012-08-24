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

#include <boost/unordered_map.hpp>

namespace Katydid
{
    class KTData;

    class KTEvent
    {
        protected:
            typedef boost::unordered_map< std::string, KTData* > DataMap;
            typedef DataMap::value_type DataMapVal;

        public:
            KTEvent();
            virtual ~KTEvent();


            //*****************************
            // Extensible data
            //*****************************
        public:
            KTData* GetData(const std::string& name) const;
            template< typename DerivedData >
            DerivedData* GetData(const std::string& name) const;

            /** @brief Adds a data object to the extensible event
             *  @details
             *    - Uses KTData::GetName() to extract the name for the key
             *    - Assumes ownership of the data.
             */
            bool AddData(KTData* newData);

            /** @brief Adds a data object to the extensible event
             *  @details
             *    - Takes a key value as a parameter for custom key selection.
             *    - Assumes ownership of the data.
             */
            bool AddData(const std::string& name, KTData* newData);

            bool RemoveData(const std::string& name);

        protected:
            DataMap fDataMap;

    };

    template< typename DerivedData >
    DerivedData* KTEvent::GetData(const std::string& name) const
    {
        KTData* dataPtr = GetData(name);
        if (dataPtr == NULL) return NULL;
        return dynamic_cast< DerivedData* >(dataPtr);
    }

    inline KTData* KTEvent::GetData(const std::string& name) const
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

    inline bool KTEvent::RemoveData(const std::string& name)
    {
        return bool(fDataMap.erase(name) > 0);
    }

} /* namespace Katydid */

#endif /* KTEVENT_HH_ */
