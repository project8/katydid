/**
 @file KTWV2DData.hh
 @brief Contains KTWV2DData
 @details 
 @author: N. S. Oblath
 @date: May 16, 2013
 */

#ifndef KTWV2DDATA_HH_
#define KTWV2DDATA_HH_

#include "KTMultiFSDataFFTW.hh"

namespace Katydid
{
    class KTWV2DData : public KTMultiFSDataFFTWCore, public KTExtensibleData< KTWV2DData >
    {
        public:
            KTWV2DData();
            virtual ~KTWV2DData();

            const std::pair< unsigned, unsigned >& GetInputPair(unsigned component = 0) const;

            void SetInputPair(unsigned first, unsigned second, unsigned component = 0);

            KTWV2DData& SetNComponents(unsigned component);

        private:
            std::vector< std::pair< unsigned, unsigned > > fWVComponentData;

            static const std::string sName;
};

    inline const std::pair< unsigned, unsigned >& KTWV2DData::GetInputPair(unsigned component) const
    {
        return fWVComponentData[component];
    }

    inline void KTWV2DData::SetInputPair(unsigned first, unsigned second, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fWVComponentData[component].first = first;
        fWVComponentData[component].second = second;
        return;
    }


} /* namespace Katydid */
#endif /* KTWV2DDATA_HH_ */
