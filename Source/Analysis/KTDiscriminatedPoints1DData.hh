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

            const SetOfPoints& GetSetOfPoints(UInt_t channelNum = 0) const;
            Double_t GetThreshold(UInt_t channelNum = 0) const;
            UInt_t GetNChannels() const;

            void AddPoint(UInt_t point, Double_t value, UInt_t channelNum = 0);
            void SetThreshold(Double_t threshold, UInt_t channelNum = 0);
            void SetNChannels(UInt_t channels);

            UInt_t GetNBins() const;
            Double_t GetBinWidth() const;

            void SetNBins(UInt_t nBins);
            void SetBinWidth(Double_t binWidth);

        protected:
            static std::string fDefaultName;

            std::vector< PerChannelData > fChannelData;

            UInt_t fNBins;
            Double_t fBinWidth;


    };

    inline const KTDiscriminatedPoints1DData::SetOfPoints& KTDiscriminatedPoints1DData::GetSetOfPoints(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fPoints;
    }

    inline Double_t KTDiscriminatedPoints1DData::GetThreshold(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fThreshold;
    }

    inline UInt_t KTDiscriminatedPoints1DData::GetNChannels() const
    {
        return UInt_t(fChannelData.size());
    }

    inline void KTDiscriminatedPoints1DData::AddPoint(UInt_t point, Double_t value, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fPoints.insert(std::make_pair(point, value));
    }

    inline void KTDiscriminatedPoints1DData::SetThreshold(Double_t threshold, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fThreshold = threshold;
    }

    inline void KTDiscriminatedPoints1DData::SetNChannels(UInt_t channels)
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


} /* namespace Katydid */

#endif /* KTDISCRIMINATEDPOINTS1DDATA_HH_ */
