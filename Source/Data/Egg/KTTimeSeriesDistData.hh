/**
 @file KTTimeSeriesDistData.hh
 @brief Contains KTTimeSeriesDistData
 @details Contains the information from a single Egg slice in the form of a 1-D std::vector of unsignedegers.
 The data are the occurances of each digitizer channel.
 @author: laroque
 @date: Jan 16, 2014
 */

#ifndef KTTIMESERIESDISTDATA_HH_
#define KTTIMESERIESDISTDATA_HH_

#include "KTData.hh"

#include "KTTimeSeriesDist.hh"

#include <vector>

namespace Katydid
{
    class KTTimeSeriesDist;

    class KTTimeSeriesDistDataCore
    {
        public:
            KTTimeSeriesDistDataCore();
            virtual ~KTTimeSeriesDistDataCore();

            unsigned GetNComponents() const;

            const KTTimeSeriesDist* GetTimeSeriesDist(unsigned component = 0) const;
            KTTimeSeriesDist* GetTimeSeriesDist(unsigned component = 0);

            virtual KTTimeSeriesDistDataCore& SetNComponents(unsigned num) = 0;

            void SetTimeSeriesDist(KTTimeSeriesDist* record, unsigned component = 0);

        protected:
            std::vector< KTTimeSeriesDist* > fTimeSeriesDist;
    };


    inline unsigned KTTimeSeriesDistDataCore::GetNComponents() const
    {
        return unsigned(fTimeSeriesDist.size());
    }

    inline KTTimeSeriesDist* KTTimeSeriesDistDataCore::GetTimeSeriesDist(unsigned component)
    {
        return fTimeSeriesDist[component];
    }

    inline const KTTimeSeriesDist* KTTimeSeriesDistDataCore::GetTimeSeriesDist(unsigned component) const
    {
        return fTimeSeriesDist[component];
    }

    inline void KTTimeSeriesDistDataCore::SetTimeSeriesDist(KTTimeSeriesDist* record, unsigned component)
    {
        if (component >= fTimeSeriesDist.size()) SetNComponents(component+1);
        fTimeSeriesDist[component] = record;
        return;
    }



    class KTTimeSeriesDistData : public KTTimeSeriesDistDataCore, public KTExtensibleData< KTTimeSeriesDistData >
    {
        public:
            KTTimeSeriesDistData();
            virtual ~KTTimeSeriesDistData();

            virtual KTTimeSeriesDistData& SetNComponents(unsigned num);
    };

} /* namespace Katydid */

#endif /* KTTIMESERIESDISTDATA_HH_ */
