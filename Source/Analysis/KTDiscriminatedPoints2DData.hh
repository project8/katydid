/*
 * KTDiscriminatedPoints2DData.hh
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#ifndef KTDISCRIMINATEDPOINTS2DDATA_HH_
#define KTDISCRIMINATEDPOINTS2DDATA_HH_

#include "KTData.hh"

#include "Rtypes.h"

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

    class KTDiscriminatedPoints2DData : public KTData
    {
        public:
            typedef std::map< std::pair< UInt_t, UInt_t >, Double_t, KTPairCompare > SetOfPoints;

        protected:
            struct PerChannelData
            {
                SetOfPoints fPoints;
                Double_t fThreshold;
            };

        public:
            KTDiscriminatedPoints2DData(UInt_t nChannels=1);
            virtual ~KTDiscriminatedPoints2DData();

            const SetOfPoints& GetSetOfPoints(UInt_t channelNum = 0) const;
            Double_t GetThreshold(UInt_t channelNum = 0) const;
            UInt_t GetNChannels() const;

            void AddPoint(UInt_t pointX, UInt_t pointY, Double_t value, UInt_t channelNum = 0);
            void SetThreshold(Double_t threshold, UInt_t channelNum = 0);
            void SetNChannels(UInt_t channels);

        protected:
            static std::string fDefaultName;

            std::vector< PerChannelData > fChannelData;

    };

    inline const KTDiscriminatedPoints2DData::SetOfPoints& KTDiscriminatedPoints2DData::GetSetOfPoints(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fPoints;
    }

    inline Double_t KTDiscriminatedPoints2DData::GetThreshold(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fThreshold;
    }

    inline UInt_t KTDiscriminatedPoints2DData::GetNChannels() const
    {
        return UInt_t(fChannelData.size());
    }

    inline void KTDiscriminatedPoints2DData::AddPoint(UInt_t pointX, UInt_t pointY, Double_t value, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fPoints.insert(std::make_pair(std::make_pair(pointX, pointY), value));
    }

    inline void KTDiscriminatedPoints2DData::SetThreshold(Double_t threshold, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fThreshold = threshold;
    }

    inline void KTDiscriminatedPoints2DData::SetNChannels(UInt_t channels)
    {
        fChannelData.resize(channels);
        return;
    }

} /* namespace Katydid */

#endif /* KTDISCRIMINATEDPOINTS2DDATA_HH_ */
