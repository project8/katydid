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
    
    class KTCorrelationData : public KTFrequencySpectrumDataPolarCore, public Nymph::KTExtensibleData< KTCorrelationData >
    {
        public:
            KTCorrelationData();
            virtual ~KTCorrelationData();

            const std::pair< unsigned, unsigned >& GetInputPair(unsigned component = 0) const;

            void SetInputPair(unsigned first, unsigned second, unsigned component = 0);

            virtual KTCorrelationData& SetNComponents(unsigned components);

        private:
            std::vector< std::pair< unsigned, unsigned > > fComponentData;

        public:
            static const std::string sName;
};

    inline const std::pair< unsigned, unsigned >& KTCorrelationData::GetInputPair(unsigned component) const
    {
        return fComponentData[component];
    }

    inline void KTCorrelationData::SetInputPair(unsigned first, unsigned second, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fComponentData[component].first = first;
        fComponentData[component].second = second;
        return;
    }


} /* namespace Katydid */
#endif /* KTCORRELATIONDATA_HH_ */
