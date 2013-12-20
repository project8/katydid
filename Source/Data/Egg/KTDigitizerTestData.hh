/*
 * KTDigitizerTestData.hh
 *
 *  Created on: Dec 18, 2013
 *      Author: nsoblath
 */

#ifndef KTDIGITIZERTESTDATA_HH_
#define KTDIGITIZERTESTDATA_HH_

#include "KTData.hh"

#include <vector>

namespace Katydid
{

    class KTDigitizerTestData : public KTExtensibleData< KTDigitizerTestData >
    {
        public:
            KTDigitizerTestData();
            virtual ~KTDigitizerTestData();

            unsigned GetNComponents() const;
            KTDigitizerTestData& SetNComponents(unsigned num);

            unsigned GetNClipTop(unsigned component = 0) const;
            unsigned GetNClipBottom(unsigned component = 0) const;
            double GetTopClipFrac(unsigned component = 0) const;
            double GetBottomClipFrac(unsigned component = 0) const;
            void SetClippingData(unsigned nClipTop, unsigned nClipBottom, double topClipFrac, double bottomClipFrac, unsigned component = 0);

        private:
            struct ClippingData
            {
                unsigned fNClipTop;
                unsigned fNClipBottom;
                double fTopClipFrac;
                double fBottomClipFrac;
            };

            std::vector< ClippingData > fClippingData;

    };

    inline unsigned KTDigitizerTestData::GetNClipTop(unsigned component) const
    {
        return fClippingData[component].fNClipTop;
    }
    inline unsigned KTDigitizerTestData::GetNClipBottom(unsigned component) const
    {
        return fClippingData[component].fNClipBottom;
    }
    inline double KTDigitizerTestData::GetTopClipFrac(unsigned component) const
    {
        return fClippingData[component].fTopClipFrac;
    }
    inline double KTDigitizerTestData::GetBottomClipFrac(unsigned component) const
    {
        return fClippingData[component].fBottomClipFrac;
    }


} /* namespace Katydid */
#endif /* KTDIGITIZERTESTDATA_HH_ */
