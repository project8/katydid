/*
 * TestDBSCAN.cc
 *
 *  Created on: Jun 24, 2014
 *      Author: nsoblath
 *
 *      Usage: ./TestDBSCAN
 */


#include "KTDBSCAN.hh"
#include "KTDistanceMatrix.hh"
#include "KTLogger.hh"

#ifdef ROOT_FOUND
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#endif


using namespace Katydid;

KTLOGGER(testlog, "TestDBSCAN");

int main()
{
    typedef KTSymmetricDistanceMatrix< double >::Point Point;
    typedef KTSymmetricDistanceMatrix< double >::Points Points;

    Points ps;

    // a scatter of points between -1 and 1 in each dimension
    // random init points dataset (dims, points)
    unsigned dims = 2;
    unsigned numPoints = 100;
    for (unsigned iPoint = 0; iPoint < numPoints; ++iPoint)
    {
        Point p(dims);
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
       Point p(dims);
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
        Point p(dims);
        for (unsigned iDim = 0; iDim < dims; ++iDim)
        {
            p(iDim) = (10.0 + rand() * (0.5) / RAND_MAX);
            //std::cout << p(i) << ' ';
        }
        ps.push_back(p);
        //std::cout << std::endl;
    }

    KTINFO(testlog, "Calculating distances");

    KTSymmetricDistanceMatrix< double > distMat;
    distMat.ComputeDistances< Euclidean< Point > >(ps);


    KTINFO(testlog, "Performing clustering");

    // init: sim threshold, minPts
    typedef KTDBSCAN< KTSymmetricDistanceMatrix< double > > DBSCAN;
    DBSCAN clustering(0.1, 10);
    DBSCAN::DBSResults results;
    clustering.DoClustering(distMat, results);

    //KTINFO(testlog, results);

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
        unsigned iClust = 0;
        for (std::vector< DBSCAN::Cluster >::const_iterator cIt = results.fClusters.begin(); cIt != results.fClusters.end(); ++cIt)
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

