/*
 * KTDBScan.cc
 *
 *  Created on: June 24, 2014
 *      Author: N.S. Oblath
 */

#include "KTDBScan.hh"

#include "KTLogger.hh"

//#include <boost/numeric/ublas/matrix_proxy.hpp>
//#include <boost/numeric/ublas/io.hpp>
//#include <boost/algorithm/minmax_element.hpp>
#include <boost/foreach.hpp>

using namespace boost::numeric;

namespace Katydid
{
    KTLOGGER(dbslog, "katydid.fft");




    KTDBScan::KTDBScan(double epsilon, unsigned minPoints) :
            fEpsilon(epsilon),
            fMinPoints(minPoints),
            fNPoints(0),
            fNoise(),
            fVisited(),
            fPointIdToClusterId(),
            fClusters(),
            fDist()
    {
    }

    KTDBScan::~KTDBScan()
    {
    }


    void KTDBScan::InitializeArrays(size_t nPoints)
    {
        fNPoints = nPoints;

        fDist.resize(fNPoints, fNPoints, false);

        fClusters.clear();
        fClusters.reserve(fNPoints);

        fVisited.resize(fNPoints, false);

        fNoise.resize(fNPoints, false);

        fPointIdToClusterId.resize(fNPoints, 0);

        return;
    }


    void KTDBScan::UniformPartition()
    {
        ClusterId cid = 0;
        for (PointId pid = 0; pid < fNPoints; ++pid)
        {
            // create a new cluster for this current point
            Cluster cluster;
            cluster.push_back(pid);
            fClusters.push_back(cluster);
            fPointIdToClusterId[pid] = cid++;
        }
    }

    KTDBScan::Neighbors KTDBScan::FindNeighbors(PointId pid, double threshold)
    {
        Neighbors neighbors;

        for (unsigned j = 0; j < fDist.size1(); ++j)
        {
            if ((pid != j) && (fDist(pid, j)) < threshold)
            {
                neighbors.push_back(j);
                //std::cout << "sim(" << pid  << "," << j << ") = " << fSim(pid, j) << ">" << threshold << std::endl;
            }
        }
        return neighbors;
    }

    bool KTDBScan::DoClustering()
    {

        ClusterId cid = 1;
        // foreach pid
        for (PointId pid = 0; pid < fNPoints; ++pid)
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
                    //std::cout << "Point i=" << pid << " can be expanded " << std::endl;// = true;

                    // Add p to current cluster

                    Cluster cluster;              // a new cluster
                    cluster.push_back(pid);       // assign pid to cluster
                    fPointIdToClusterId[pid] = cid;

                    // go to neighbors
                    for (unsigned int i = 0; i < ne.size(); ++i)
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
                                //std::cout << "\t Expanding to pid=" << nPid << std::endl;
                                // join
                                BOOST_FOREACH(Neighbors::value_type n1, ne1)
                                {
                                    // join neighbors
                                    ne.push_back(n1);
                                    //std::cerr << "\tPushback pid=" << n1 << std::endl;
                                }
                                //std::cout << std::endl;
                            }
                        }

                        // not already assigned to a cluster
                        if (!fPointIdToClusterId[nPid])
                        {
                            //std::cout << "\tadding pid=" << nPid << std::endl;
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

    // single point output
    std::ostream& operator<<(std::ostream& stream, const KTDBScan::Point& point)
    {
        stream << "{ ";
        BOOST_FOREACH(KTDBScan::Point::value_type x, point)
        {
            stream << " " << x;
        }
        stream << " }, ";

        return stream;
    }

    // single cluster output
    std::ostream& operator<<(std::ostream& stream, const KTDBScan::Cluster& cluster)
    {
        stream << "[ ";
        BOOST_FOREACH(KTDBScan::PointId pid, cluster)
        {
            stream << " " << pid;
        }
        stream << " ]";

        return stream;
    }

    // clusters output
    std::ostream& operator<<(std::ostream& stream, const KTDBScan& cs)
    {
        KTDBScan::ClusterId cid = 1;
        BOOST_FOREACH(KTDBScan::Cluster c, cs.fClusters)
        {
            stream << "c(" << cid++ << ") = " << c << std::endl;
            /*
            BOOST_FOREACH(KTDBScan::PointId pid, c)
            {
                stream << cs.fPoints[pid];
            }
            stream << std::endl;
            */
        }
        return stream;
    }

} /* namespace Katydid */
