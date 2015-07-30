/**
 @file KTTimeSeriesData.hh
 @brief Contains KTTimeSeriesData
 @details Contains the information from a single Egg slice in the form of a 1-D std::vector of unsignedegers.
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
#include <iostream>

namespace Katydid
{
    using namespace Nymph;
    class KTTimeSeries;

    class KTTimeSeriesDataCore
    {
        public:
            KTTimeSeriesDataCore();
            virtual ~KTTimeSeriesDataCore();

            unsigned GetNComponents() const;

            const KTTimeSeries* GetTimeSeries(unsigned component = 0) const;
            KTTimeSeries* GetTimeSeries(unsigned component = 0);

            virtual KTTimeSeriesDataCore& SetNComponents(unsigned num) = 0;

            void SetTimeSeries(KTTimeSeries* record, unsigned component = 0);

        protected:
            std::vector< KTTimeSeries* > fTimeSeries;
    };


    inline unsigned KTTimeSeriesDataCore::GetNComponents() const
    {
        return unsigned(fTimeSeries.size());
    }

    inline KTTimeSeries* KTTimeSeriesDataCore::GetTimeSeries(unsigned component)
    {
        return fTimeSeries[component];
    }

    inline const KTTimeSeries* KTTimeSeriesDataCore::GetTimeSeries(unsigned component) const
    {
        return fTimeSeries[component];
    }

    inline void KTTimeSeriesDataCore::SetTimeSeries(KTTimeSeries* record, unsigned component)
    {
        if (component >= fTimeSeries.size()) SetNComponents(component+1);
        if (fTimeSeries[component] != NULL) delete fTimeSeries[component];
        fTimeSeries[component] = record;
        return;
    }



    class KTTimeSeriesData : public KTTimeSeriesDataCore, public KTExtensibleData< KTTimeSeriesData >
    {
        public:
            KTTimeSeriesData();
            virtual ~KTTimeSeriesData();

            virtual KTTimeSeriesData& SetNComponents(unsigned num);

        public:
            static const std::string sName;
    };

} /* namespace Katydid */

#endif /* KTTIMESERIESDATA_HH_ */
