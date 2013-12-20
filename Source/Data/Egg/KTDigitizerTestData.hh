/*
 * KTDigitizerTestData.hh
 *
 *  Created on: Dec 18, 2013
 *      Author: nsoblath
 */

#ifndef KTDIGITIZERTESTDATA_HH_
#define KTDIGITIZERTESTDATA_HH_

#include "KTData.hh"

#include "KTCountHistogram.hh"

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

            unsigned GetNBits() const;
            void SetNBits(unsigned nBits);

            bool GetBitOccupancyFlag() const;
            void SetBitOccupancyFlag(bool flag);
            KTCountHistogram* GetBitHistogram(unsigned component = 0) const;
            void AddBits(unsigned value, unsigned component = 0);

            bool GetClippingFlag() const;
            void SetClippingFlag(bool flag);
            unsigned GetNClipTop(unsigned component = 0) const;
            unsigned GetNClipBottom(unsigned component = 0) const;
            double GetTopClipFrac(unsigned component = 0) const;
            double GetBottomClipFrac(unsigned component = 0) const;
            void SetClippingData(unsigned nClipTop, unsigned nClipBottom, double topClipFrac, double bottomClipFrac, unsigned component = 0);

        private:
            unsigned fNBits;

            struct BitOccupancyData
            {
                KTCountHistogram* fBitHistogram;
            };

            bool fBitOccupancyFlag;
            std::vector< BitOccupancyData > fBitOccupancyData;

            struct ClippingData
            {
                unsigned fNClipTop;
                unsigned fNClipBottom;
                double fTopClipFrac;
                double fBottomClipFrac;
            };

            bool fClippingFlag;
            std::vector< ClippingData > fClippingData;

    };

    inline unsigned KTDigitizerTestData::GetNBits() const
    {
        return fNBits;
    }

    inline bool KTDigitizerTestData::GetBitOccupancyFlag() const
    {
        return fBitOccupancyFlag;
    }
    inline void KTDigitizerTestData::SetBitOccupancyFlag(bool flag)
    {
        fBitOccupancyFlag = flag;
        return;
    }
    inline KTCountHistogram* KTDigitizerTestData::GetBitHistogram(unsigned component) const
    {
        return fBitOccupancyData[component].fBitHistogram;
    }

    inline bool KTDigitizerTestData::GetClippingFlag() const
    {
        return fClippingFlag;
    }
    inline void KTDigitizerTestData::SetClippingFlag(bool flag)
    {
        fClippingFlag = flag;
        return;
    }
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
