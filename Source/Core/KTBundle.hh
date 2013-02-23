/**
 @file KTBundle.hh
 @brief Contains KTBundle
 @details An bundle represents the information recorded over a certain period of time.
 An bundle is extensible: it can include a variety of types of data (i.e. from a particular analysis)
 The added data objects are stored in an unordered map.
 @note: Prior to August 24, 2012, KTBundle was the current KTTimeSeriesData.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTBUNDLE_HH_
#define KTBUNDLE_HH_

#include "KTDataMap.hh"
#include "KTTIFactory.hh"
#include "KTLogger.hh"

#include "Rtypes.h"

#include <boost/unordered_map.hpp>

namespace Katydid
{
    KTLOGGER(corelog_bundle, "katydid.core");

    class KTData;

    class KTBundle
    {
        protected:
            typedef boost::unordered_map< const std::type_info*, KTDataMap* > MapOfDataMaps;
            typedef boost::unordered_map< std::string, KTDataMap* > DataNameMap;

        public:
            KTBundle();
            virtual ~KTBundle();

            //*****************************
            // Intrinsic data
            //*****************************
        public:
            unsigned GetBundleNumber() const;
            void SetBundleNumber(unsigned num);

            Bool_t GetIsLastBundle() const;
            void SetIsLastBundle(Bool_t flag);

        protected:
            unsigned fBundleNum;
            Bool_t fIsLastBundle;

            //*****************************
            // Extensible data
            //*****************************
        public:
            //KTData* GetData(const std::string& name) const;

            /** @brief Adds a data object to the extensible bundle
             *  @details
             *    - Uses KTData::GetName() to extract the name for the key
             *    - Assumes ownership of the data.
             */
            template< typename DerivedData >
            Bool_t AddData(DerivedData* newData);

            /** @brief Adds a data object to the extensible bundle
             *  @details
             *    - Takes a key value as a parameter for custom key selection.
             *    - Assumes ownership of the data.
             */
            template< typename DerivedData >
            Bool_t AddData(const std::string& name, DerivedData* newData);

            /// Finds the data object specified and returns a pointer to it
            template< typename DerivedData >
            DerivedData* GetData(const std::string& name) const;

            /// Finds the data object specified and returns a pointer to it; ownership is transferred to the caller
            template< typename DerivedData >
            DerivedData* ExtractData(const std::string& name) const;

            /// Deletes the data object specified
            Bool_t RemoveData(const std::string& name);

            void PrintAttachedData() const;

        protected:
            KTTIFactory< KTDataMap >* fDataMapFactory; // singleton; not owned by KTBundle

            MapOfDataMaps fMapOfDataMaps;

            DataNameMap fDataNameMap;

    };


    inline unsigned KTBundle::GetBundleNumber() const
    {
        return fBundleNum;
    }

    inline void KTBundle::SetBundleNumber(unsigned num)
    {
        fBundleNum = num;
        return;
    }

    inline Bool_t KTBundle::GetIsLastBundle() const
    {
        return fIsLastBundle;
    }

    inline void KTBundle::SetIsLastBundle(Bool_t flag)
    {
        fIsLastBundle = flag;
        return;
    }

    template< typename DerivedData >
    Bool_t KTBundle::AddData(DerivedData* newData)
    {
        return AddData(newData->GetName(), newData);
    }

    template< typename DerivedData >
    Bool_t KTBundle::AddData(const std::string& name, DerivedData* newData)
    {
        if (fDataNameMap.find(name) != fDataNameMap.end())
        {
            KTERROR(corelog_bundle, "There is already a data object named <" << name << "> attached to this bundle.");
            return false;
        }

        MapOfDataMaps::const_iterator modmIt = fMapOfDataMaps.find(&typeid(KTDerivedDataMap< DerivedData >));
        if (modmIt == fMapOfDataMaps.end())
        {
            KTERROR(corelog_bundle, "Attempt to add data called <" << name << "> failed because there is no data class registered under map type <" << typeid(KTDerivedDataMap< DerivedData >).name() << ">");
            return false;
        }
        // If the data map has been found, we can reasonably assume its derived type will matched the derived data type.

        KTDerivedDataMap< DerivedData >* ddm = static_cast< KTDerivedDataMap< DerivedData >* >(modmIt->second);
        if (! ddm->AddData(name, newData))
        {
            KTERROR(corelog_bundle, "Attempt to add data called <" << name << "> to data map failed.");
            return false;
        }

        newData->fBundle = this;
        return true;
    }


    template< typename DerivedData >
    DerivedData* KTBundle::GetData(const std::string& name) const
    {
        MapOfDataMaps::const_iterator modmIt = fMapOfDataMaps.find(&typeid(KTDerivedDataMap< DerivedData >));
        if (modmIt == fMapOfDataMaps.end())
        {
            KTERROR(corelog_bundle, "Attempt to retrieve data called <" << name << "> failed because there is no data class registered under map type <" << typeid(KTDerivedDataMap< DerivedData >).name() << ">");
            return NULL;
        }

        KTData* dataPtr = modmIt->second->GetData(name);
        if (dataPtr == NULL)
        {
            //KTERROR(corelog_bundle, "Unable to find data called <" << name << "> with type <" << typeid(DerivedData).name() << ">");
            return NULL;
        }
        return static_cast< DerivedData* >(dataPtr);
    }

    template< typename DerivedData >
    DerivedData* KTBundle::ExtractData(const std::string& name) const
    {
        MapOfDataMaps::const_iterator modmIt = fMapOfDataMaps.find(&typeid(KTDerivedDataMap< DerivedData >));
        if (modmIt == fMapOfDataMaps.end())
        {
            KTERROR(corelog_bundle, "Attempt to extract data called <" << name << "> failed because there is no data class registered under map type <" << typeid(KTDerivedDataMap< DerivedData >).name() << ">");
            return NULL;
        }

        KTData* dataPtr = modmIt->second->ExtractData(name);
        if (dataPtr == NULL)
        {
            //KTERROR(corelog_bundle, "Unable to find data called <" << name << "> with type <" << typeid(DerivedData).name() << ">");
            return NULL;
        }
        return static_cast< DerivedData* >(dataPtr);
    }

    inline Bool_t KTBundle::RemoveData(const std::string& name)
    {
        DataNameMap::const_iterator nmIt = fDataNameMap.find(name);
        if (nmIt == fDataNameMap.end()) return false;

        fDataNameMap.erase(name);
        if (! nmIt->second->RemoveData(name))
        {
            KTWARN(corelog_bundle, "While removing data called <" << name << ">, data was not found in the data map");
        }
        return true;
    }

} /* namespace Katydid */

#endif /* KTBUNDLE_HH_ */
