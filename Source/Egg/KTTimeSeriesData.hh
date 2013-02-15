/**
 @file KTTimeSeriesData.hh
 @brief Contains KTTimeSeriesData
 @details Contains the information from a single Egg bundle in the form of a 1-D std::vector of UInt_tegers.
 The data are the time series of the bundle.
 @note Prior to August 24, 2012, this class was called KTBundle.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTTIMESERIESDATA_HH_
#define KTTIMESERIESDATA_HH_

#include "KTData.hh"

#include <vector>

namespace Katydid
{
    class KTTimeSeries;

    class KTTimeSeriesData : public KTExtensibleData< KTTimeSeriesData >
    {
        public:
            KTTimeSeriesData();
            virtual ~KTTimeSeriesData();

            UInt_t GetNComponents() const;

            const KTTimeSeries* GetTimeSeries(UInt_t component = 0) const;
            KTTimeSeries* GetTimeSeries(UInt_t component = 0);

            KTTimeSeriesData& SetNComponents(UInt_t num);

            void SetTimeSeries(KTTimeSeries* record, UInt_t component = 0);

        protected:
            std::vector< KTTimeSeries* > fComponentData;
    };

    inline UInt_t KTTimeSeriesData::GetNComponents() const
    {
        return UInt_t(fComponentData.size());
    }

    inline KTTimeSeries* KTTimeSeriesData::GetTimeSeries(UInt_t component)
    {
        return fComponentData[component];
    }

    inline const KTTimeSeries* KTTimeSeriesData::GetTimeSeries(UInt_t component) const
    {
        return fComponentData[component];
    }

    inline KTTimeSeriesData& KTTimeSeriesData::SetNComponents(UInt_t num)
    {
        fComponentData.resize(num);
        return *this;
    }

    inline void KTTimeSeriesData::SetTimeSeries(KTTimeSeries* record, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component] = record;
        return;
    }


} /* namespace Katydid */

#endif /* KTTIMESERIESDATA_HH_ */
