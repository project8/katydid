/**
 @file KTCorrelationData.hh
 @brief Contains KTCorrelationData
 @details
 @author: N. S. Oblath
 @date: Aug  20, 2012
 */

#ifndef KTCORRELATIONDATA_HH_
#define KTCORRELATIONDATA_HH_

#include "KTFrequencySpectrumDataPolar.hh"

namespace Katydid
{
    class KTCorrelationData : public KTFrequencySpectrumDataPolarCore, public KTExtensibleData< KTCorrelationData >
    {
        public:
            KTCorrelationData();
            virtual ~KTCorrelationData();

            inline const std::pair< UInt_t, UInt_t >& GetInputPair(UInt_t component = 0) const
            {
                return fComponentData[component];
            }

            inline void SetInputPair(UInt_t first, UInt_t second, UInt_t component = 0)
            {
                if (component >= fSpectra.size()) SetNComponents(component+1);
                fComponentData[component].first = first;
                fComponentData[component].second = second;
                return;
            }

            virtual KTCorrelationData& SetNComponents(UInt_t components);

        protected:
            std::vector< std::pair< UInt_t, UInt_t > > fComponentData;
    };

    inline const std::pair< UInt_t, UInt_t >& KTCorrelationData::GetInputPair(UInt_t component = 0) const
    {
        return fComponentData[component];
    }

    inline void KTCorrelationData::SetInputPair(UInt_t first, UInt_t second, UInt_t component = 0)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fComponentData[component].first = first;
        fComponentData[component].second = second;
        return;
    }


} /* namespace Katydid */
#endif /* KTCORRELATIONDATA_HH_ */
