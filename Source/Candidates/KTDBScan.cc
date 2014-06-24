/*
 * KTDBScan.cc
 *
 *  Created on: June 24, 2014
 *      Author: N.S. Oblath
 */

#include "KTDBScan.hh"

#include "KTLogger.hh"

#include <boost/foreach.hpp>


namespace Katydid
{
    KTLOGGER(tclog, "katydid.fft");

    KTDBScan::KTDBScan(double epsilon, unsigned minPoints) :
            fEpsilon(epsilon),
            fMinPoints(minPoints),
            fNoise(),
            fVisited(),
            fPoints(),
            fPointIdToClusterId(),
            fClusters(),
            fSim()
    {
    }

    KTDBScan::~KTDBScan()
    {
    }

    bool KTDBScan::TakePoints(const Points& points)
    {
        for (Points::const_iterator it = points.begin(); it != points.end(); ++it)
        {
            fPoints.push_back(*it);
        }
        return true;
    }

    bool KTDBScan::TakePoint(const Point& point)
    {
        fPoints.push_back(point);
        return true;
    }

    void KTDBScan::ResetPoints()
    {
        fPoints.clear();
        return;
    }

    void KTDBScan::UniformPartition()
    {
        PointId pid = 0;
        ClusterId cid = 0;
        fClusters.clear();
        fClusters.reserve(fPoints.size());
        BOOST_FOREACH(Point point, fPoints)
        {
            // create a new cluster for this current point
            Cluster cluster;
            cluster.push_back(pid++);
            fClusters.push_back(cluster);
            fPointIdToClusterId.push_back(cid++);
        }
    }

    KTDBScan::Neighbors KTDBScan::FindNeighbors(PointId pid, double threshold)
    {
        Neighbors neighbors;

        for (unsigned j = 0; j < fSim.size1(); ++j)
        {
            if ((pid != j) && (fSim(pid, j)) > threshold)
            {
                neighbors.push_back(j);
                //std::cout << "sim(" << pid  << "," << j << ")" << _sim(pid, j) << ">" << threshold << std::endl;
            }
        }
        return neighbors;
    }

    bool KTDBScan::DoClustering()
    {
        fVisited.clear();
        fVisited.resize(fPoints.size(), false);
        fNoise.clear();
        fNoise.resize(fPoints.size(), false);

        ClusterId cid = 1;
        // foreach pid
        for (PointId pid = 0; pid < fPoints.size(); pid++)
        {
            // not already visited
            if (! fVisited[pid])
            {

                fVisited[pid] = true;

                // get the neighbors
                Neighbors ne = FindNeighbors(pid, fEpsilon);

                // not enough support -> mark as noise
                if (ne.size() < fMinPoints)
                {
                    fNoise[pid] = true;
                }
                else
                {
                    std::cout << "Point i=" << pid << " can be expanded " << std::endl;// = true;

                    // Add p to current cluster

                    Cluster cluster;              // a new cluster
                    cluster.push_back(pid);       // assign pid to cluster
                    fPointIdToClusterId[pid] = cid;

                    // go to neighbors
                    for (unsigned int i = 0; i < ne.size(); i++)
                    {
                        PointId nPid = ne[i];

                        // not already visited
                        if (! fVisited[nPid])
                        {
                            fVisited[nPid] = true;

                            // go to neighbors
                            Neighbors ne1 = FindNeighbors(nPid, fEpsilon);

                            // enough support
                            if (ne1.size() >= fMinPoints)
                            {
                                std::cout << "\t Expanding to pid=" << nPid << std::endl;
                                // join
                                BOOST_FOREACH(Neighbors::value_type n1, ne1)
                                {
                                    // join neighbors
                                    ne.push_back(n1);
                                    std::cerr << "\tPushback pid=" << n1 << std::endl;
                                }
                                std::cout << std::endl;
                            }
                        }

                        // not already assigned to a cluster
                        if (!fPointIdToClusterId[nPid])
                        {
                            std::cout << "\tadding pid=" << nPid << std::endl;
                            cluster.push_back(nPid);
                            fPointIdToClusterId[nPid]=cid;
                        }
                    }

                    fClusters.push_back(cluster);
                    cid++;
                }
            } // if (!visited)
        } // for

        return true;
    }

} /* namespace Katydid */
