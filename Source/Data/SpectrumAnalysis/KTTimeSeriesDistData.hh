/*
 * KTTimeSeriesDistData.hh
 *
 *  Created on: Jan 22, 2014
 *      Author: laroque
 */

#ifndef KTTIMESERIESDISTDATA_HH_
#define KTTIMESERIESDISTDATA_HH_

#include "KTData.hh"
#include "KTTimeSeriesDist.hh"

#include <vector>

namespace Katydid
{
    using namespace Nymph;
    //class KTTimeSeriesDist;

    class KTTimeSeriesDistData : public KTExtensibleData< KTTimeSeriesDistData >
    {
        public:
            KTTimeSeriesDistData();
            virtual ~KTTimeSeriesDistData();

            unsigned GetNComponents() const;
            KTTimeSeriesDistData& SetNComponents(unsigned num);

            const KTTimeSeriesDist* GetTimeSeriesDist(unsigned component = 0) const;
            KTTimeSeriesDist* GetTimeSeriesDist(unsigned component = 0);
            void SetTimeSeriesDist(KTTimeSeriesDist* record, unsigned component = 0);

        private:
            std::vector< KTTimeSeriesDist* > fTimeSeriesDist;

        public:
            static const std::string sName;
    };

    inline unsigned KTTimeSeriesDistData::GetNComponents() const
    {
        return unsigned(fTimeSeriesDist.size());
    }

    inline KTTimeSeriesDist* KTTimeSeriesDistData::GetTimeSeriesDist(unsigned component)
    {
        return fTimeSeriesDist[component];
    }

    inline const KTTimeSeriesDist* KTTimeSeriesDistData::GetTimeSeriesDist(unsigned component) const
    {
        return fTimeSeriesDist[component];
    }

    inline void KTTimeSeriesDistData::SetTimeSeriesDist(KTTimeSeriesDist* record, unsigned component)
    {
        if (component >= fTimeSeriesDist.size()) SetNComponents(component+1);
        fTimeSeriesDist[component] = record;
        return;
    }

} /* namespace Katydid */
#endif /* KTTIMESERIESDISTDATA_HH_ */
