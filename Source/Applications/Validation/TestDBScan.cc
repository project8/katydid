/*
 * TestDBScan.cc
 *
 *  Created on: Jun 24, 2014
 *      Author: nsoblath
 *
 *      Usage: ./TestDBScan
 */


#include "KTDBScan.hh"
#include "KTLogger.hh"

#ifdef ROOT_FOUND
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#endif


using namespace Katydid;

KTLOGGER(testlog, "TestDBScan");

int main()
{
/*
    DBSCAN::ClusterData cl_d = DBSCAN::gen_cluster_data( 2, 1000 );

    DBSCAN dbs(0.1, 5, 1);

    dbs.fit( cl_d );

    std::cout << dbs << std::endl;
*/



    KTDBScan::Points ps;

    // a scatter of points between -1 and 1 in each dimension
    // random init points dataset (dims, points)
    unsigned dims = 2;
    unsigned numPoints = 100;
    for (unsigned iPoint = 0; iPoint < numPoints; ++iPoint)
    {
        KTDBScan::Point p(dims);
        for (unsigned iDim = 0; iDim < dims; ++iDim)
        {
            p(iDim) = (-1.0 + rand() * (2.0) / RAND_MAX);
            //std::cout << p(i) << ' ';
        }
        ps.push_back(p);
        //std::cout << std::endl;
    }

    // a tight cluster of points around -10 in each dimension
    for (unsigned iPoint = 0; iPoint < numPoints; ++iPoint)
    {
        KTDBScan::Point p(dims);
        for (unsigned iDim = 0; iDim < dims; ++iDim)
        {
            p(iDim) = (-10.0 + rand() * (0.5) / RAND_MAX);
            //std::cout << p(i) << ' ';
        }
        ps.push_back(p);
        //std::cout << std::endl;
    }

    // a tight cluster of points around 10 in each dimension
    for (unsigned iPoint = 0; iPoint < numPoints; ++iPoint)
    {
        KTDBScan::Point p(dims);
        for (unsigned iDim = 0; iDim < dims; ++iDim)
        {
            p(iDim) = (10.0 + rand() * (0.5) / RAND_MAX);
            //std::cout << p(i) << ' ';
        }
        ps.push_back(p);
        //std::cout << std::endl;
    }


    // init: sim threshold, minPts
    KTDBScan clustering(0.1, 10);
    clustering.RunDBScan< Euclidean< KTDBScan::Point > >(ps);

    KTINFO(testlog, clustering);

#ifdef ROOT_FOUND
    if (dims == 2)
    {
        TFile file("dbscan_test.root", "recreate");
        TCanvas* canv = new TCanvas("cClusters", "Clusters");

        TGraph* ptsGraph = new TGraph(ps.size());
        ptsGraph->SetMarkerStyle(1);
        ptsGraph->SetMarkerColor(1);
        for (unsigned pid = 0; pid < ps.size(); ++pid)
        {
            ptsGraph->SetPoint(pid, ps[pid](0), ps[pid](1));
        }
        ptsGraph->Draw("ap");
        ptsGraph->Write("Points");

        unsigned firstClusterColor = 2;
        const std::vector< KTDBScan::Cluster >& clusters = clustering.GetClusters();
        unsigned iClust = 0;
        for (std::vector< KTDBScan::Cluster >::const_iterator cIt = clusters.begin(); cIt != clusters.end(); ++cIt)
        {
            TGraph* clGraph = new TGraph(cIt->size());
            clGraph->SetMarkerStyle(4);
            clGraph->SetMarkerColor(firstClusterColor + iClust);
            for (unsigned iPt = 0; iPt < cIt->size(); ++iPt)
            {
                clGraph->SetPoint(iPt, ps[(*cIt)[iPt]](0), ps[(*cIt)[iPt]](1));
            }
            std::stringstream nameStream;
            nameStream << "Cluster" << iClust;
            clGraph->Draw("psame");
            clGraph->Write(nameStream.str().c_str());

            iClust++;
        }

        canv->Write();

        file.Close();
    }
#endif


    return 0;
}

