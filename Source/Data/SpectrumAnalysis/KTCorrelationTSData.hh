/**
 @file KTCorrelationTSData.hh
 @brief Contains KTCorrelationTSData
 @details
 @author: N. S. Oblath
 @date: Aug  9, 2013
 */

#ifndef KTCORRELATIONTSDATA_HH_
#define KTCORRELATIONTSDATA_HH_

#include "KTTimeSeriesData.hh"

namespace Katydid
{
    using namespace Nymph;
    class KTCorrelationTSData : public KTTimeSeriesDataCore, public KTExtensibleData< KTCorrelationTSData >
    {
        public:
            KTCorrelationTSData();
            virtual ~KTCorrelationTSData();

            const std::pair< unsigned, unsigned >& GetInputPair(unsigned component = 0) const;

            void SetInputPair(unsigned first, unsigned second, unsigned component = 0);

            virtual KTCorrelationTSData& SetNComponents(unsigned components);

        private:
            std::vector< std::pair< unsigned, unsigned > > fComponentData;

        public:
            static const std::string sName;
};

    inline const std::pair< unsigned, unsigned >& KTCorrelationTSData::GetInputPair(unsigned component) const
    {
        return fComponentData[component];
    }

    inline void KTCorrelationTSData::SetInputPair(unsigned first, unsigned second, unsigned component)
    {
        if (component >= fTimeSeries.size()) SetNComponents(component+1);
        fComponentData[component].first = first;
        fComponentData[component].second = second;
        return;
    }


} /* namespace Katydid */
#endif /* KTCORRELATIONTSDATA_HH_ */
