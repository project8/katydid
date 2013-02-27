/**
 @file KTTimeSeriesData.hh
 @brief Contains KTTimeSeriesData
 @details Contains the information from a single Egg slice in the form of a 1-D std::vector of UInt_tegers.
 The data are the time series of the slice.
 @note Prior to August 24, 2012, this class was called KTEvent.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTTIMESERIESDATA_HH_
#define KTTIMESERIESDATA_HH_

#include "KTData.hh"

#include "KTTimeSeries.hh"

#include <vector>

namespace Katydid
{
    class KTTimeSeries;

    class KTTimeSeriesDataCore
    {
        public:
            KTTimeSeriesDataCore();
            virtual ~KTTimeSeriesDataCore();

            UInt_t GetNComponents() const;

            const KTTimeSeries* GetTimeSeries(UInt_t component = 0) const;
            KTTimeSeries* GetTimeSeries(UInt_t component = 0);

            virtual KTTimeSeriesDataCore& SetNComponents(UInt_t num) = 0;

            void SetTimeSeries(KTTimeSeries* record, UInt_t component = 0);

        protected:
            std::vector< KTTimeSeries* > fTimeSeries;
    };


    inline UInt_t KTTimeSeriesDataCore::GetNComponents() const
    {
        return UInt_t(fTimeSeries.size());
    }

    inline KTTimeSeries* KTTimeSeriesDataCore::GetTimeSeries(UInt_t component)
    {
        return fTimeSeries[component];
    }

    inline const KTTimeSeries* KTTimeSeriesDataCore::GetTimeSeries(UInt_t component) const
    {
        return fTimeSeries[component];
    }

    inline void KTTimeSeriesDataCore::SetTimeSeries(KTTimeSeries* record, UInt_t component)
    {
        if (component >= fTimeSeries.size()) SetNComponents(component+1);
        fTimeSeries[component] = record;
        return;
    }



    class KTTimeSeriesData : public KTTimeSeriesDataCore, public KTExtensibleData< KTTimeSeriesData >
    {
        public:
            KTTimeSeriesData();
            virtual ~KTTimeSeriesData();

            virtual KTTimeSeriesData& SetNComponents(UInt_t num);
    };

    inline KTTimeSeriesData& KTTimeSeriesData::SetNComponents(UInt_t num)
    {
        fTimeSeries.resize(num);
        return *this;
    }



} /* namespace Katydid */

#endif /* KTTIMESERIESDATA_HH_ */
