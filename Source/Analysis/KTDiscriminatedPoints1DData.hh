/*
 * KTDiscriminatedPoints1DData.hh
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#ifndef KTDISCRIMINATEDPOINTS1DDATA_HH_
#define KTDISCRIMINATEDPOINTS1DDATA_HH_

#include "KTData.hh"

#include "Rtypes.h"

#include <map>
#include <utility>
#include <vector>

namespace Katydid
{

    class KTDiscriminatedPoints1DData : public KTData
    {
        public:
            typedef std::map< UInt_t, Double_t > SetOfPoints;

        protected:
            struct PerChannelData
            {
                SetOfPoints fPoints;
                Double_t fThreshold;
            };

        public:
            KTDiscriminatedPoints1DData(UInt_t nChannels=1);
            virtual ~KTDiscriminatedPoints1DData();

            const SetOfPoints& GetSetOfPoints(UInt_t component = 0) const;
            Double_t GetThreshold(UInt_t component = 0) const;
            UInt_t GetNComponents() const;
            Double_t GetTimeInRun() const;
            ULong64_t GetSliceNumber() const;

            void AddPoint(UInt_t point, Double_t value, UInt_t component = 0);
            void SetThreshold(Double_t threshold, UInt_t component = 0);
            void SetNComponents(UInt_t channels);
            void SetTimeInRun(Double_t tir);
            void SetSliceNumber(ULong64_t slice);

            UInt_t GetNBins() const;
            Double_t GetBinWidth() const;

            void SetNBins(UInt_t nBins);
            void SetBinWidth(Double_t binWidth);

        protected:
            static std::string fDefaultName;

            std::vector< PerChannelData > fChannelData;

            UInt_t fNBins;
            Double_t fBinWidth;

            Double_t fTimeInRun;
            ULong64_t fSliceNumber;

    };

    inline const KTDiscriminatedPoints1DData::SetOfPoints& KTDiscriminatedPoints1DData::GetSetOfPoints(UInt_t component) const
    {
        return fChannelData[component].fPoints;
    }

    inline Double_t KTDiscriminatedPoints1DData::GetThreshold(UInt_t component) const
    {
        return fChannelData[component].fThreshold;
    }

    inline UInt_t KTDiscriminatedPoints1DData::GetNComponents() const
    {
        return UInt_t(fChannelData.size());
    }

    inline Double_t KTDiscriminatedPoints1DData::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline ULong64_t KTDiscriminatedPoints1DData::GetSliceNumber() const
    {
        return fSliceNumber;
    }

    inline void KTDiscriminatedPoints1DData::AddPoint(UInt_t point, Double_t value, UInt_t component)
    {
        if (component >= fChannelData.size()) fChannelData.resize(component+1);
        fChannelData[component].fPoints.insert(std::make_pair(point, value));
    }

    inline void KTDiscriminatedPoints1DData::SetThreshold(Double_t threshold, UInt_t component)
    {
        if (component >= fChannelData.size()) fChannelData.resize(component+1);
        fChannelData[component].fThreshold = threshold;
    }

    inline void KTDiscriminatedPoints1DData::SetNComponents(UInt_t channels)
    {
        fChannelData.resize(channels);
        return;
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

    inline void KTDiscriminatedPoints1DData::SetTimeInRun(Double_t tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTDiscriminatedPoints1DData::SetSliceNumber(ULong64_t slice)
    {
        fSliceNumber = slice;
        return;
    }

} /* namespace Katydid */

#endif /* KTDISCRIMINATEDPOINTS1DDATA_HH_ */
