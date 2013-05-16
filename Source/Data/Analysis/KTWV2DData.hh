/**
 @file KTWV2DData.hh
 @brief Contains KTWV2DData
 @details 
 @author: N. S. Oblath
 @date: May 16, 2013
 */

#ifndef KTWV2DDATA_HH_
#define KTWV2DDATA_HH_

//#include "KTTimeFrequencyDataFFTW.hh"
#include "KTTimeFrequencyDataPolar.hh"

namespace Katydid
{
    //class KTWV2DData : public KTTimeFrequencyDataFFTWCore, public KTExtensibleData< KTWV2DData >
    class KTWV2DData : public KTTimeFrequencyDataPolarCore, public KTExtensibleData< KTWV2DData >
    {
        public:
            KTWV2DData();
            virtual ~KTWV2DData();

            const std::pair< UInt_t, UInt_t >& GetInputPair(UInt_t component = 0) const;

            void SetInputPair(UInt_t first, UInt_t second, UInt_t component = 0);

            virtual KTWV2DData& SetNComponents(UInt_t components);

        protected:
            std::vector< std::pair< UInt_t, UInt_t > > fWVComponentData;
    };

    inline const std::pair< UInt_t, UInt_t >& KTWV2DData::GetInputPair(UInt_t component) const
    {
        return fWVComponentData[component];
    }

    inline void KTWV2DData::SetInputPair(UInt_t first, UInt_t second, UInt_t component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fWVComponentData[component].first = first;
        fWVComponentData[component].second = second;
        return;
    }


} /* namespace Katydid */
#endif /* KTWV2DDATA_HH_ */
