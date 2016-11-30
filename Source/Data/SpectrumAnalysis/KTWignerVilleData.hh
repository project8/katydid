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
    
    class KTWignerVilleData : public KTFrequencySpectrumDataFFTWCore, public Nymph::KTExtensibleData< KTWignerVilleData >
    {
        public:
            KTWignerVilleData();
            virtual ~KTWignerVilleData();

            const std::pair< unsigned, unsigned >& GetInputPair(unsigned component = 0) const;

            void SetInputPair(unsigned first, unsigned second, unsigned component = 0);

            virtual KTWignerVilleData& SetNComponents(unsigned components);

        private:
            std::vector< std::pair< unsigned, unsigned > > fWVComponentData;

        public:
            static const std::string sName;
};

    inline const std::pair< unsigned, unsigned >& KTWignerVilleData::GetInputPair(unsigned component) const
    {
        return fWVComponentData[component];
    }

    inline void KTWignerVilleData::SetInputPair(unsigned first, unsigned second, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fWVComponentData[component].first = first;
        fWVComponentData[component].second = second;
        return;
    }


} /* namespace Katydid */
#endif /* KTWIGNERVILLEDATA_HH_ */
