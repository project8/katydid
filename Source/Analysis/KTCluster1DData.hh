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
        public:
            typedef std::pair< UInt_t, UInt_t > Cluster; // first unsigned: first bin in cluster; second unsigned: last bin in cluster

        protected:
            struct ClusterCompare
            {
                    bool operator() (const Cluster& lhs, const Cluster& rhs)
                    {
                        return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.first < rhs.first);
                    }
            };

        public:
            typedef std::set< Cluster, ClusterCompare > SetOfClusters;

        protected:
            struct PerGroupData
            {
                SetOfClusters fClusters;
                Double_t fThreshold;
            };

        public:
            KTCluster1DData(UInt_t nGroups=1);
            virtual ~KTCluster1DData();

            const SetOfClusters& GetSetOfClusters(UInt_t groupNum = 0) const;
            Double_t GetThreshold(UInt_t groupNum = 0) const;
            UInt_t GetNGroups() const;

            void AddCluster(UInt_t firstPoint, UInt_t lastPoint, UInt_t groupNum = 0);
            void AddClusters(const SetOfClusters& clusters, UInt_t groupNum = 0);
            void SetThreshold(Double_t threshold, UInt_t groupNum = 0);
            void SetNGroups(UInt_t channels);

            UInt_t GetNBins() const;
            Double_t GetBinWidth() const;

            void SetNBins(UInt_t nBins);
            void SetBinWidth(Double_t binWidth);

        protected:
            static std::string fDefaultName;

            std::vector< PerGroupData > fGroupData;

            UInt_t fNBins;
            Double_t fBinWidth;


    };

    inline const KTCluster1DData::SetOfClusters& KTCluster1DData::GetSetOfClusters(UInt_t groupNum) const
    {
        return fGroupData[groupNum].fClusters;
    }

    inline Double_t KTCluster1DData::GetThreshold(UInt_t groupNum) const
    {
        return fGroupData[groupNum].fThreshold;
    }

    inline UInt_t KTCluster1DData::GetNGroups() const
    {
        return UInt_t(fGroupData.size());
    }

    inline void KTCluster1DData::AddCluster(UInt_t firstPoint, UInt_t lastPoint, UInt_t groupNum)
    {
        if (groupNum >= fGroupData.size()) fGroupData.resize(groupNum+1);
        fGroupData[groupNum].fClusters.insert(std::make_pair(firstPoint, lastPoint));
    }

    inline void KTCluster1DData::SetThreshold(Double_t threshold, UInt_t groupNum)
    {
        if (groupNum >= fGroupData.size()) fGroupData.resize(groupNum+1);
        fGroupData[groupNum].fThreshold = threshold;
    }

    inline void KTCluster1DData::AddClusters(const SetOfClusters& clusters, UInt_t groupNum)
    {
        if (groupNum >= fGroupData.size()) fGroupData.resize(groupNum+1);
        fGroupData[groupNum].fClusters.insert(clusters.begin(), clusters.end());
        return;
    }

    inline void KTCluster1DData::SetNGroups(UInt_t channels)
    {
        fGroupData.resize(channels);
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
