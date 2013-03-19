/**
 @file KTWignerVilleData.hh
 @brief Contains KTWignerVilleData
 @details 
 @author: N. S. Oblath
 @date: Oct 19, 2012
 */

#ifndef KTWIGNERVILLEDATA_HH_
#define KTWIGNERVILLEDATA_HH_

#include "KTFrequencySpectrumDataFFTW.hh"

namespace Katydid
{
    class KTWignerVilleData : public KTFrequencySpectrumDataFFTWCore, public KTExtensibleData< KTWignerVilleData >
    {
        public:
            KTWignerVilleData();
            virtual ~KTWignerVilleData();

            const std::pair< UInt_t, UInt_t >& GetInputPair(UInt_t component = 0) const;

            void SetInputPair(UInt_t first, UInt_t second, UInt_t component = 0);

            virtual KTWignerVilleData& SetNComponents(UInt_t components);

        protected:
            std::vector< std::pair< UInt_t, UInt_t > > fWVComponentData;
    };

    inline const std::pair< UInt_t, UInt_t >& KTWignerVilleData::GetInputPair(UInt_t component) const
    {
        return fWVComponentData[component];
    }

    inline void KTWignerVilleData::SetInputPair(UInt_t first, UInt_t second, UInt_t component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fWVComponentData[component].first = first;
        fWVComponentData[component].second = second;
        return;
    }


} /* namespace Katydid */
#endif /* KTWIGNERVILLEDATA_HH_ */
