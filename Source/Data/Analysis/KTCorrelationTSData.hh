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
    class KTCorrelationTSData : public KTTimeSeriesDataCore, public KTExtensibleData< KTCorrelationTSData >
    {
        public:
            KTCorrelationTSData();
            virtual ~KTCorrelationTSData();

            const std::pair< UInt_t, UInt_t >& GetInputPair(UInt_t component = 0) const;

            void SetInputPair(UInt_t first, UInt_t second, UInt_t component = 0);

            virtual KTCorrelationTSData& SetNComponents(UInt_t components);

        protected:
            std::vector< std::pair< UInt_t, UInt_t > > fComponentData;
    };

    inline const std::pair< UInt_t, UInt_t >& KTCorrelationTSData::GetInputPair(UInt_t component) const
    {
        return fComponentData[component];
    }

    inline void KTCorrelationTSData::SetInputPair(UInt_t first, UInt_t second, UInt_t component)
    {
        if (component >= fTimeSeries.size()) SetNComponents(component+1);
        fComponentData[component].first = first;
        fComponentData[component].second = second;
        return;
    }


} /* namespace Katydid */
#endif /* KTCORRELATIONTSDATA_HH_ */
