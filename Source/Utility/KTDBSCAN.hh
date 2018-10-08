/**
 @file KTDBSCAN.hh
 @brief Contains KTDBSCAN
 @details DBSCAN Clustering Algorithm
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
    
    KTLOGGER(dbslog_h, "KTDBSCAN");


    /*!
     @class KTDBSCAN
     @author N.S. Oblath

     @brief DBSCAN Clustering Algorithm

     @details
     This implementation was adapted from:
     http://codingplayground.blogspot.com/2009/11/dbscan-clustering-algorithm.html
     Accessed on 6/5/2014.
     Code was provided without a license.
     */

    template< typename DistanceData >
    class KTDBSCAN
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
            KTDBSCAN(double radius = 1., unsigned minPoints = 1);
            virtual ~KTDBSCAN();

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
    KTDBSCAN< DistanceData >::KTDBSCAN(double radius, unsigned minPoints) :
            fRadius(radius),
            fMinPoints(minPoints),
            fVisited()
    {
    }

    template< typename DistanceData >
    KTDBSCAN< DistanceData >::~KTDBSCAN()
    {
    }

    template< typename DistanceData >
    inline double KTDBSCAN< DistanceData >::GetRadius() const
    {
        return fRadius;
    }

    template< typename DistanceData >
    inline void KTDBSCAN< DistanceData >::SetRadius(double eps)
    {
        fRadius = eps;
        return;
    }

    template< typename DistanceData >
    inline unsigned KTDBSCAN< DistanceData >::GetMinPoints() const
    {
        return fMinPoints;
    }

    template< typename DistanceData >
    inline void KTDBSCAN< DistanceData >::SetMinPoints(unsigned pts)
    {
        fMinPoints = pts;
        return;
    }


    template< typename DistanceData >
    void KTDBSCAN< DistanceData >::InitializeArrays(size_t nPoints, DBSResults& results)
    {
        KTINFO(dbslog_h, "Initializing DBSCAN arrays with " << nPoints << " points");

        results.fClusters.clear();
        results.fClusters.reserve(nPoints);

        results.fNoise.resize(nPoints, false);

        results.fPointIdToClusterId.resize(nPoints, -1);

        fVisited.resize(nPoints, false);

        return;
    }

    template< typename DistanceData >
    bool KTDBSCAN< DistanceData >::DoClustering(const DistanceData& dist, DBSResults& results)
    {
        KTDEBUG(dbslog_h, "Starting DBSCAN; min points: " << fMinPoints << "; radius: " << fRadius);
        PointId nPoints = dist.size();

        InitializeArrays(nPoints, results);

        ClusterId cid = 0;
        // foreach pid
        for (PointId pid = 0; pid < nPoints; ++pid)
        {
            //KTWARN(dbslog_h, "Visiting point " << pid);
            // not already visited
            if (! fVisited[pid])
            {
                fVisited[pid] = true;

                // get the neighbors
                Neighbors ne = dist.NearestNeighborsByRadius(pid, fRadius);
                //KTWARN(dbslog_h, "This is a new point; it has " << ne.size() << " neighbors");

                // not enough support -> mark as noise
                if (ne.size() < fMinPoints)
                {
                    //KTWARN(dbslog_h, "That's below the threshold; it's a noise point");
                    results.fNoise[pid] = true;
                }
                else
                {
                    // Add p to current cluster
                    Cluster cluster;              // a new cluster
                    cluster.push_back(pid);       // assign pid to cluster
                    results.fPointIdToClusterId[pid] = cid;
                    //KTWARN(dbslog_h, "Starting cluster with this point; cluster is " << cid);

                    // go to neighbors
                    for (unsigned int i = 0; i < ne.size(); ++i)
                    {
                        //KTWARN(dbslog_h, "    Visiting neighbor, point " << ne[i]);
                        PointId nPid = ne[i];

                        // not already visited
                        if (! fVisited[nPid])
                        {
                            fVisited[nPid] = true;

                            // go to neighbors
                            Neighbors ne1 = dist.NearestNeighborsByRadius(nPid, fRadius);
                            //KTWARN(dbslog_h, "    This is a new point; it has " << ne1.size() << " neighbors");

                            // enough support
                            if (ne1.size() >= fMinPoints)
                            {
                                //KTWARN(dbslog_h, "    Joining neighbor's neighbors with the previous cluster");
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
                            //KTWARN(dbslog_h, "    Neighbor was not part of a cluster; adding to cluster " << cid);
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
