/*
 * KTRawTimeSeriesData.hh
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#ifndef KTRAWTIMESERIES_HH_
#define KTRAWTIMESERIES_HH_

#include "KTData.hh"

namespace Katydid
{
    class KTTimeSeriesRaw;

    class KTRawTimeSeriesData : public KTExtensibleData< KTRawTimeSeriesData >
    {
        public:
            KTRawTimeSeriesData();
            virtual ~KTRawTimeSeriesData();

            unsigned GetNComponents() const;
            KTRawTimeSeriesData& SetNComponents(unsigned num);

            const KTTimeSeriesRaw* GetTimeSeries(unsigned component = 0) const;
            KTTimeSeriesRaw* GetTimeSeries(unsigned component = 0);
            void SetTimeSeries(KTTimeSeriesRaw* record, unsigned component = 0);

        protected:
            std::vector< KTTimeSeriesRaw* > fTimeSeries;

    };

    inline unsigned KTRawTimeSeriesData::GetNComponents() const
    {
        return unsigned(fTimeSeries.size());
    }

    inline KTTimeSeriesRaw* KTRawTimeSeriesData::GetTimeSeries(unsigned component)
    {
        return fTimeSeries[component];
    }

    inline const KTTimeSeriesRaw* KTRawTimeSeriesData::GetTimeSeries(unsigned component) const
    {
        return fTimeSeries[component];
    }

    inline void KTRawTimeSeriesData::SetTimeSeries(KTTimeSeriesRaw* record, unsigned component)
    {
        if (component >= fTimeSeries.size()) SetNComponents(component+1);
        fTimeSeries[component] = record;
        return;
    }


} /* namespace Katydid */
#endif /* KTRAWTIMESERIES_HH_ */
