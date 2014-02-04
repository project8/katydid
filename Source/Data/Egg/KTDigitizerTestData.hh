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
            unsigned GetNMultClipTop(unsigned component = 0) const;
            unsigned GetNMultClipBottom(unsigned component = 0) const;
            double GetTopClipFrac(unsigned component = 0) const;
            double GetBottomClipFrac(unsigned component = 0) const;
            double GetMultTopClipFrac(unsigned component = 0) const;
            double GetMultBottomClipFrac(unsigned component = 0) const;
            void SetClippingData(unsigned nClipTop, unsigned nClipBottom, unsigned nMultClipTop, unsigned nMultClipBottom, double topClipFrac, double bottomClipFrac, double multTopClipFrac, double multBottomClipFrac, unsigned component = 0);

            bool GetLinearityFlag() const;
            void SetLinearityFlag(bool flat);
            double GetFracMaxDiff(unsigned component = 0) const;
            double GetChiSquared(unsigned component = 0) const;
            size_t GetFitStart(unsigned componenet = 0) const;
            size_t GetFitEnd(unsigned component = 0) const;
            double GetRegSlope(unsigned component = 0) const;
            double GetRegIntercept(unsigned component = 0) const;
            void SetLinearityData(double fracMaxDiff, double chiSquared, size_t fitStart, size_t fitEnd, double regSlope, double regIntercept, unsigned component = 0);


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
	        unsigned fNMultClipTop;
	        unsigned fNMultClipBottom;
                double fTopClipFrac;
                double fBottomClipFrac;
	        double fMultTopClipFrac;
	        double fMultBottomClipFrac;

            };

            bool fClippingFlag;
            std::vector< ClippingData > fClippingData;

            struct LinearityData
	    {
	         double fFracMaxDiff;
	         double fChiSquared;
	         size_t fFitStart;
	         size_t fFitEnd;
	         double fRegSlope;
	         double fRegIntercept;
	    };

            bool fLinearityFlag;
            std::vector< LinearityData > fLinearityData;

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
    inline unsigned KTDigitizerTestData::GetNMultClipTop(unsigned component) const
    {
        return fClippingData[component].fNMultClipTop;
    }
    inline unsigned KTDigitizerTestData::GetNMultClipBottom(unsigned component) const
    {
        return fClippingData[component].fNMultClipBottom;
    }
    inline double KTDigitizerTestData::GetTopClipFrac(unsigned component) const
    {
        return fClippingData[component].fTopClipFrac;
    }
    inline double KTDigitizerTestData::GetBottomClipFrac(unsigned component) const
    {
        return fClippingData[component].fBottomClipFrac;
    }
    inline double KTDigitizerTestData::GetMultTopClipFrac(unsigned component) const
    {
        return fClippingData[component].fMultTopClipFrac;
    }
    inline double KTDigitizerTestData::GetMultBottomClipFrac(unsigned component) const
    {
        return fClippingData[component].fMultBottomClipFrac;
    }

    inline bool KTDigitizerTestData::GetLinearityFlag() const
    {
        return fLinearityFlag;
    }
    inline void KTDigitizerTestData::SetLinearityFlag(bool flag)
    {
        fLinearityFlag = flag;
        return;
    }
    inline double KTDigitizerTestData::GetFracMaxDiff(unsigned component) const
    {
        return fLinearityData[component].fFracMaxDiff;
    }
    inline double KTDigitizerTestData::GetChiSquared(unsigned component) const
    {
        return fLinearityData[component].fChiSquared;
    }
    inline size_t  KTDigitizerTestData::GetFitStart(unsigned component) const
    {
        return fLinearityData[component].fFitStart;
    }
    inline size_t KTDigitizerTestData::GetFitEnd(unsigned component) const
    {
        return fLinearityData[component].fFitEnd;
    }
    inline double KTDigitizerTestData::GetRegSlope(unsigned component) const
    {
        return fLinearityData[component].fRegSlope;
    }
    inline double KTDigitizerTestData::GetRegIntercept(unsigned component) const
    {
        return fLinearityData[component].fRegIntercept;
    }


} /* namespace Katydid */
#endif /* KTDIGITIZERTESTDATA_HH_ */
