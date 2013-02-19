/*
 * KTDiscriminatedPoints1DData.hh
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#ifndef KTDISCRIMINATEDPOINTS1DDATA_HH_
#define KTDISCRIMINATEDPOINTS1DDATA_HH_

#include "KTData.hh"

#include <map>
#include <utility>
#include <vector>

namespace Katydid
{

    class KTDiscriminatedPoints1DData : public KTData< KTDiscriminatedPoints1DData >
    {
        public:
            typedef std::map< UInt_t, Double_t > SetOfPoints;

        protected:
            struct PerComponentData
            {
                SetOfPoints fPoints;
                Double_t fThreshold;
            };

        public:
            KTDiscriminatedPoints1DData();
            virtual ~KTDiscriminatedPoints1DData();

            const SetOfPoints& GetSetOfPoints(UInt_t component = 0) const;
            Double_t GetThreshold(UInt_t component = 0) const;

            UInt_t GetNComponents() const;

            void AddPoint(UInt_t point, Double_t value, UInt_t component = 0);
            void SetThreshold(Double_t threshold, UInt_t component = 0);

            KTDiscriminatedPoints1DData& SetNComponents(UInt_t channels);

            UInt_t GetNBins() const;
            Double_t GetBinWidth() const;

            void SetNBins(UInt_t nBins);
            void SetBinWidth(Double_t binWidth);

        protected:
            static std::string fDefaultName;

            std::vector< PerComponentData > fComponentData;

            UInt_t fNBins;
            Double_t fBinWidth;

    };

    inline const KTDiscriminatedPoints1DData::SetOfPoints& KTDiscriminatedPoints1DData::GetSetOfPoints(UInt_t component) const
    {
        return fComponentData[component].fPoints;
    }

    inline Double_t KTDiscriminatedPoints1DData::GetThreshold(UInt_t component) const
    {
        return fComponentData[component].fThreshold;
    }

    inline UInt_t KTDiscriminatedPoints1DData::GetNComponents() const
    {
        return UInt_t(fComponentData.size());
    }

    inline void KTDiscriminatedPoints1DData::AddPoint(UInt_t point, Double_t value, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fPoints.insert(std::make_pair(point, value));
    }

    inline void KTDiscriminatedPoints1DData::SetThreshold(Double_t threshold, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fThreshold = threshold;
    }

    inline KTDiscriminatedPoints1DData& KTDiscriminatedPoints1DData::SetNComponents(UInt_t channels)
    {
        fComponentData.resize(channels);
        return *this;
    }

    inline UInt_t KTDiscriminatedPoints1DData::GetNBins() const
    {
        return fNBins;
    }

    inline Double_t KTDiscriminatedPoints1DData::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTDiscriminatedPoints1DData::SetNBins(UInt_t nBins)
    {
        fNBins = nBins;
        return;
    }

    inline void KTDiscriminatedPoints1DData::SetBinWidth(Double_t binWidth)
    {
        fBinWidth = binWidth;
        return;
    }

} /* namespace Katydid */

#endif /* KTDISCRIMINATEDPOINTS1DDATA_HH_ */
