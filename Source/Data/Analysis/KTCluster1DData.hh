/*
 * KTCluster1DData.hh
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#ifndef KTCLUSTER1DDATA_HH_
#define KTCLUSTER1DDATA_HH_

#include "KTData.hh"

#include <set>
#include <utility>
#include <vector>

namespace Katydid
{
    class KTCluster1DData : public KTExtensibleData< KTCluster1DData >
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
            struct PerComponentData
            {
                SetOfClusters fClusters;
                Double_t fThreshold;
            };

        public:
            KTCluster1DData();
            virtual ~KTCluster1DData();

            const SetOfClusters& GetSetOfClusters(UInt_t component = 0) const;
            Double_t GetThreshold(UInt_t component = 0) const;

            UInt_t GetNComponents() const;

            void AddCluster(UInt_t firstPoint, UInt_t lastPoint, UInt_t component = 0);
            void AddClusters(const SetOfClusters& clusters, UInt_t component = 0);
            void SetThreshold(Double_t threshold, UInt_t component = 0);

            KTCluster1DData& SetNComponents(UInt_t components);

            UInt_t GetNBins() const;
            Double_t GetBinWidth() const;

            void SetNBins(UInt_t nBins);
            void SetBinWidth(Double_t binWidth);

        protected:
            std::vector< PerComponentData > fComponentData;

            UInt_t fNBins;
            Double_t fBinWidth;


    };

    inline const KTCluster1DData::SetOfClusters& KTCluster1DData::GetSetOfClusters(UInt_t component) const
    {
        return fComponentData[component].fClusters;
    }

    inline Double_t KTCluster1DData::GetThreshold(UInt_t component) const
    {
        return fComponentData[component].fThreshold;
    }

    inline UInt_t KTCluster1DData::GetNComponents() const
    {
        return UInt_t(fComponentData.size());
    }

    inline void KTCluster1DData::AddCluster(UInt_t firstPoint, UInt_t lastPoint, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fClusters.insert(std::make_pair(firstPoint, lastPoint));
    }

    inline void KTCluster1DData::SetThreshold(Double_t threshold, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fThreshold = threshold;
    }

    inline void KTCluster1DData::AddClusters(const SetOfClusters& clusters, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fClusters.insert(clusters.begin(), clusters.end());
        return;
    }

    inline KTCluster1DData& KTCluster1DData::SetNComponents(UInt_t components)
    {
        fComponentData.resize(components);
        return *this;
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
