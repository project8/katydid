/*
 * KTDiscriminatedPoints2DData.hh
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#ifndef KTDISCRIMINATEDPOINTS2DDATA_HH_
#define KTDISCRIMINATEDPOINTS2DDATA_HH_

#include "KTData.hh"

#include <map>
#include <utility>
#include <vector>

namespace Katydid
{
    struct KTPairCompare
    {
        bool operator() (const std::pair< UInt_t, UInt_t >& lhs, const std::pair< UInt_t, UInt_t >& rhs)
        {
            return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.first < rhs.first);
        }
    };

    class KTDiscriminatedPoints2DData : public KTExtensibleData< KTDiscriminatedPoints2DData >
    {
        public:
            typedef std::map< std::pair< UInt_t, UInt_t >, Double_t, KTPairCompare > SetOfPoints;

        protected:
            struct PerComponentData
            {
                SetOfPoints fPoints;
                Double_t fThreshold;
            };

        public:
            KTDiscriminatedPoints2DData();
            virtual ~KTDiscriminatedPoints2DData();

            const SetOfPoints& GetSetOfPoints(UInt_t component = 0) const;
            Double_t GetThreshold(UInt_t component = 0) const;

            UInt_t GetNComponents() const;

            void AddPoint(UInt_t pointX, UInt_t pointY, Double_t value, UInt_t component = 0);
            void SetThreshold(Double_t threshold, UInt_t component = 0);

            KTDiscriminatedPoints2DData& SetNComponents(UInt_t channels);

            UInt_t GetNBinsX() const;
            UInt_t GetNBinsY() const;
            Double_t GetBinWidthX() const;
            Double_t GetBinWidthY() const;

            void SetNBinsX(UInt_t nBins);
            void SetNBinsY(UInt_t nBins);
            void SetBinWidthX(Double_t binWidth);
            void SetBinWidthY(Double_t binWidth);

        protected:
            std::vector< PerComponentData > fComponentData;

            UInt_t fNBinsX;
            UInt_t fNBinsY;
            Double_t fBinWidthX;
            Double_t fBinWidthY;

    };

    inline const KTDiscriminatedPoints2DData::SetOfPoints& KTDiscriminatedPoints2DData::GetSetOfPoints(UInt_t component) const
    {
        return fComponentData[component].fPoints;
    }

    inline Double_t KTDiscriminatedPoints2DData::GetThreshold(UInt_t component) const
    {
        return fComponentData[component].fThreshold;
    }

    inline UInt_t KTDiscriminatedPoints2DData::GetNComponents() const
    {
        return UInt_t(fComponentData.size());
    }

    inline void KTDiscriminatedPoints2DData::AddPoint(UInt_t pointX, UInt_t pointY, Double_t value, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fPoints.insert(std::make_pair(std::make_pair(pointX, pointY), value));
    }

    inline void KTDiscriminatedPoints2DData::SetThreshold(Double_t threshold, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fThreshold = threshold;
    }

    inline KTDiscriminatedPoints2DData& KTDiscriminatedPoints2DData::SetNComponents(UInt_t channels)
    {
        fComponentData.resize(channels);
        return *this;
    }

    inline UInt_t KTDiscriminatedPoints2DData::GetNBinsX() const
    {
        return fNBinsX;
    }

    inline UInt_t KTDiscriminatedPoints2DData::GetNBinsY() const
    {
        return fNBinsY;
    }

    inline Double_t KTDiscriminatedPoints2DData::GetBinWidthX() const
    {
        return fBinWidthX;
    }

    inline Double_t KTDiscriminatedPoints2DData::GetBinWidthY() const
    {
        return fBinWidthY;
    }

    inline void KTDiscriminatedPoints2DData::SetNBinsX(UInt_t nBins)
    {
        fNBinsX = nBins;
        return;
    }

    inline void KTDiscriminatedPoints2DData::SetNBinsY(UInt_t nBins)
    {
        fNBinsY = nBins;
        return;
    }

    inline void KTDiscriminatedPoints2DData::SetBinWidthX(Double_t binWidth)
    {
        fBinWidthX = binWidth;
        return;
    }

    inline void KTDiscriminatedPoints2DData::SetBinWidthY(Double_t binWidth)
    {
        fBinWidthY = binWidth;
        return;
    }

} /* namespace Katydid */

#endif /* KTDISCRIMINATEDPOINTS2DDATA_HH_ */
