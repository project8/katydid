/*
 * KTRawTimeSeriesData.hh
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#ifndef KTRAWTIMESERIESDATA_HH_
#define KTRAWTIMESERIESDATA_HH_

#include "Data.hh"

#include <vector>

namespace Katydid
{
    
    class KTRawTimeSeries;

    class KTRawTimeSeriesData : public Nymph::Data
    {
        public:
            KTRawTimeSeriesData();
            virtual ~KTRawTimeSeriesData();

            unsigned GetNComponents() const;
            KTRawTimeSeriesData& SetNComponents(unsigned num);

            const KTRawTimeSeries* GetTimeSeries(unsigned component = 0) const;
            KTRawTimeSeries* GetTimeSeries(unsigned component = 0);
            void SetTimeSeries(KTRawTimeSeries* record, unsigned component = 0);

        private:
            std::vector< KTRawTimeSeries* > fTimeSeries;
    };

    inline unsigned KTRawTimeSeriesData::GetNComponents() const
    {
        return unsigned(fTimeSeries.size());
    }

    inline KTRawTimeSeries* KTRawTimeSeriesData::GetTimeSeries(unsigned component)
    {
        return fTimeSeries[component];
    }

    inline const KTRawTimeSeries* KTRawTimeSeriesData::GetTimeSeries(unsigned component) const
    {
        return fTimeSeries[component];
    }

    inline void KTRawTimeSeriesData::SetTimeSeries(KTRawTimeSeries* record, unsigned component)
    {
        if (component >= fTimeSeries.size()) SetNComponents(component+1);
        fTimeSeries[component] = record;
        return;
    }


} /* namespace Katydid */
#endif /* KTRAWTIMESERIESDATA_HH_ */
