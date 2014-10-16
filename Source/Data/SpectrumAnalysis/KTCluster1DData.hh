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
            typedef std::pair< unsigned, unsigned > Cluster; // first unsigned: first bin in cluster; second unsigned: last bin in cluster

        protected:
            struct ClusterCompare
            {
                    bool operator() (const Cluster& lhs, const Cluster& rhs)
                    {
                        return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
                    }
            };

        public:
            typedef std::set< Cluster, ClusterCompare > SetOfClusters;

        protected:
            struct PerComponentData
            {
                SetOfClusters fClusters;
                double fThreshold;
            };

        public:
            KTCluster1DData();
            virtual ~KTCluster1DData();

            const SetOfClusters& GetSetOfClusters(unsigned component = 0) const;
            double GetThreshold(unsigned component = 0) const;

            unsigned GetNComponents() const;

            void AddCluster(unsigned firstPoint, unsigned lastPoint, unsigned component = 0);
            void AddClusters(const SetOfClusters& clusters, unsigned component = 0);
            void SetThreshold(double threshold, unsigned component = 0);

            KTCluster1DData& SetNComponents(unsigned components);

            unsigned GetNBins() const;
            double GetBinWidth() const;

            void SetNBins(unsigned nBins);
            void SetBinWidth(double binWidth);

        private:
            std::vector< PerComponentData > fComponentData;

            unsigned fNBins;
            double fBinWidth;

        public:
            static const std::string sName;

    };

    inline const KTCluster1DData::SetOfClusters& KTCluster1DData::GetSetOfClusters(unsigned component) const
    {
        return fComponentData[component].fClusters;
    }

    inline double KTCluster1DData::GetThreshold(unsigned component) const
    {
        return fComponentData[component].fThreshold;
    }

    inline unsigned KTCluster1DData::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline void KTCluster1DData::AddCluster(unsigned firstPoint, unsigned lastPoint, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fClusters.insert(std::make_pair(firstPoint, lastPoint));
    }

    inline void KTCluster1DData::SetThreshold(double threshold, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fThreshold = threshold;
    }

    inline void KTCluster1DData::AddClusters(const SetOfClusters& clusters, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fClusters.insert(clusters.begin(), clusters.end());
        return;
    }

    inline KTCluster1DData& KTCluster1DData::SetNComponents(unsigned components)
    {
        fComponentData.resize(components);
        return *this;
    }

    inline unsigned KTCluster1DData::GetNBins() const
    {
        return fNBins;
    }

    inline double KTCluster1DData::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTCluster1DData::SetNBins(unsigned nBins)
    {
        fNBins = nBins;
        return;
    }

    inline void KTCluster1DData::SetBinWidth(double binWidth)
    {
        fBinWidth = binWidth;
        return;
    }

} /* namespace Katydid */

#endif /* KTCLUSTER1DDATA_HH_ */
