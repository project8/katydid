/**
 @file KTDBScan.hh
 @brief Contains KTDBScan
 @details DBScan Clustering Algorithm
 @author: N.S. Oblath
 @date: Jun 24, 2014
 */

#ifndef KTDBSCAN_HH_
#define KTDBSCAN_HH_

#include <cmath>
#include <vector>

#include "KTLogger.hh"

namespace Katydid
{
    
    KTLOGGER(dbslog, "KTDBScan");


    /*!
     @class KTDBScan
     @author N.S. Oblath

     @brief DBSCAN Clustering Algorithm

     @details
     This implementation was adapted from:
     http://codingplayground.blogspot.com/2009/11/dbscan-clustering-algorithm.html
     Accessed on 6/5/2014.
     Code was provided without a license.
     */

    template< typename DistanceData >
    class KTDBScan
    {
        public:
            typedef int ClusterId;

            //typedef unsigned PointId;
            typedef typename DistanceData::PointId PointId;

            // a cluster is a vector of PointId
            typedef std::vector< PointId > Cluster;

            // a set of Neighbors is a vector of PointId
            typedef typename DistanceData::Neighbors Neighbors;

            struct DBSResults
            {
                    // which points are noise
                    std::vector< bool > fNoise;

                    // mapping point_id -> clusterId
                    std::vector< ClusterId > fPointIdToClusterId;

                    // the collection of clusters
                    std::vector< Cluster > fClusters;
            };

        public:
            KTDBScan(double radius = 1., unsigned minPoints = 1);
            virtual ~KTDBScan();

            double GetRadius() const;
            void SetRadius(double eps);

            unsigned GetMinPoints() const;
            void SetMinPoints(unsigned pts);

        private:
            // Two points are neighbors if the distance
            // between them does not exceed radius value.
            double fRadius;

            //minimum number of points
            unsigned fMinPoints;

        public:
            bool DoClustering(const DistanceData& dist, DBSResults& results);

        private:
            void InitializeArrays(size_t nPoints, DBSResults& results);

            // visited-point vector
            std::vector< bool > fVisited;

    };

    template< typename DistanceData >
    KTDBScan< DistanceData >::KTDBScan(double radius, unsigned minPoints) :
            fRadius(radius),
            fMinPoints(minPoints),
            fVisited()
    {
    }

    template< typename DistanceData >
    KTDBScan< DistanceData >::~KTDBScan()
    {
    }

    template< typename DistanceData >
    inline double KTDBScan< DistanceData >::GetRadius() const
    {
        return fRadius;
    }

    template< typename DistanceData >
    inline void KTDBScan< DistanceData >::SetRadius(double eps)
    {
        fRadius = eps;
        return;
    }

    template< typename DistanceData >
    inline unsigned KTDBScan< DistanceData >::GetMinPoints() const
    {
        return fMinPoints;
    }

    template< typename DistanceData >
    inline void KTDBScan< DistanceData >::SetMinPoints(unsigned pts)
    {
        fMinPoints = pts;
        return;
    }


    template< typename DistanceData >
    void KTDBScan< DistanceData >::InitializeArrays(size_t nPoints, DBSResults& results)
    {
        KTINFO(dbslog, "Initializing DBSCAN arrays with " << nPoints << " points");

        results.fClusters.clear();
        results.fClusters.reserve(nPoints);

        results.fNoise.resize(nPoints, false);

        results.fPointIdToClusterId.resize(nPoints, -1);

        fVisited.resize(nPoints, false);

        return;
    }

    template< typename DistanceData >
    bool KTDBScan< DistanceData >::DoClustering(const DistanceData& dist, DBSResults& results)
    {
        PointId nPoints = dist.size();

        InitializeArrays(nPoints, results);

        ClusterId cid = 0;
        // foreach pid
        for (PointId pid = 0; pid < nPoints; ++pid)
        {
            // not already visited
            if (! fVisited[pid])
            {

                fVisited[pid] = true;

                // get the neighbors
                Neighbors ne = dist.NearestNeighborsByRadius(pid, fRadius);

                // not enough support -> mark as noise
                if (ne.size() < fMinPoints)
                {
                    results.fNoise[pid] = true;
                }
                else
                {
                    // Add p to current cluster

                    Cluster cluster;              // a new cluster
                    cluster.push_back(pid);       // assign pid to cluster
                    results.fPointIdToClusterId[pid] = cid;

                    // go to neighbors
                    for (unsigned int i = 0; i < ne.size(); ++i)
                    {
                        PointId nPid = ne[i];

                        // not already visited
                        if (! fVisited[nPid])
                        {
                            fVisited[nPid] = true;

                            // go to neighbors
                            Neighbors ne1 = dist.NearestNeighborsByRadius(nPid, fRadius);

                            // enough support
                            if (ne1.size() >= fMinPoints)
                            {
                                // join
                                for (unsigned int j = 0; j < ne1.size(); ++j)
                                {
                                    ne.push_back(ne1[j]);
                                }
                            }
                        }

                        // not already assigned to a cluster
                        if (results.fPointIdToClusterId[nPid] == -1)
                        {
                            cluster.push_back(nPid);
                            results.fPointIdToClusterId[nPid] = cid;
                        }
                    }

                    results.fClusters.push_back(cluster);
                    ++cid;
                }
            } // if (!visited)
        } // for

        return true;
    }

} /* namespace Katydid */
#endif /* KTDBSCAN_HH_ */
