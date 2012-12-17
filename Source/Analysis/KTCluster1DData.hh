/*
 * KTCluster1DData.hh
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#ifndef KTCLUSTER1DDATA_HH_
#define KTCLUSTER1DDATA_HH_

#include "KTData.hh"

#include "Rtypes.h"

#include <set>
#include <utility>
#include <vector>

namespace Katydid
{
    class KTCluster1DData : public KTData
    {
        protected:
            struct PairCompare
            {
                    bool operator() (const std::pair< UInt_t, UInt_t >& lhs, const std::pair< UInt_t, UInt_t >& rhs)
                    {
                        return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.first < rhs.first);
                    }
            };

        public:
            typedef std::set< std::pair< UInt_t, UInt_t>, PairCompare > SetOfClusters;

        protected:
            struct PerChannelData
            {
                SetOfClusters fClusters;
                Double_t fThreshold;
            };

        public:
            KTCluster1DData(UInt_t nChannels=1);
            virtual ~KTCluster1DData();

            const SetOfClusters& GetSetOfClusters(UInt_t channelNum = 0) const;
            Double_t GetThreshold(UInt_t channelNum = 0) const;
            UInt_t GetNChannels() const;

            void AddPoint(UInt_t minPoint, UInt_t maxPoint, UInt_t channelNum = 0);
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

    inline const KTCluster1DData::SetOfClusters& KTCluster1DData::GetSetOfClusters(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fClusters;
    }

    inline Double_t KTCluster1DData::GetThreshold(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fThreshold;
    }

    inline UInt_t KTCluster1DData::GetNChannels() const
    {
        return UInt_t(fChannelData.size());
    }

    inline void KTCluster1DData::AddPoint(UInt_t minPoint, UInt_t maxPoint, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fClusters.insert(std::make_pair(minPoint, maxPoint));
    }

    inline void KTCluster1DData::SetThreshold(Double_t threshold, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fThreshold = threshold;
    }

    inline void KTCluster1DData::SetNChannels(UInt_t channels)
    {
        fChannelData.resize(channels);
        return;
    }

    inline UInt_t KTCluster1DData::GetNBins() const
    {
        return fNBins;
    }

    inline Double_t KTCluster1DData::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTCluster1DData::SetNBins(UInt_t nBins)
    {
        fNBins = nBins;
        return;
    }

    inline void KTCluster1DData::SetBinWidth(Double_t binWidth)
    {
        fBinWidth = binWidth;
        return;
    }

} /* namespace Katydid */

#endif /* KTCLUSTER1DDATA_HH_ */
