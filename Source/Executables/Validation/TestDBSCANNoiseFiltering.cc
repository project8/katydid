/*
 * TestDBSCANNoiseFiltering.cc
 *
 *  Created on: Aug 8, 2018
 *      Author: N.S. Oblath
 */

#include "KTDBSCANNoiseFiltering.hh"
#include "KTKDTreeData.hh"
#include "KTLogger.hh"

#ifdef ROOT_FOUND
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#endif


using namespace Katydid;

KTLOGGER(testlog, "TestDBSCANNoiseFiltering");

int main()
{
    // parameters
    std::pair< double, double > range( 0., 1. );
    unsigned nNoisePoints = 1000;

    std::pair< double, double > trackXRange( 0.2, 0.3 );
    std::pair< double, double > trackYRange( 0.6, 0.61 );
    unsigned nTrackPoints = 100;

    KTINFO(testlog, "Creating data");

    using KTKDTreeData::Point;
    KTPointCloud< Point > points;
    points.fPoints.reserve(nNoisePoints + nTrackPoints);

    double delta = range.second - range.first;
    for (unsigned iPoint = 0; iPoint < nNoisePoints; ++iPoint)
    {
        Point pt;

        pt.fCoords[0] = range.first + rand() * delta / RAND_MAX;
        pt.fCoords[1] = range.first + rand() * delta / RAND_MAX;

        points.fPoints.push_back(pt);
    }

    double deltaX = trackXRange.second - trackXRange.first;
    double deltaY = trackYRange.second - trackYRange.first;
    for (unsigned iPoint = 0; iPoint < nTrackPoints; ++iPoint)
    {
        Point pt;

        pt.fCoords[0] = trackXRange.first + rand() * deltaX / RAND_MAX;
        pt.fCoords[1] = trackYRange.first + rand() * deltaY / RAND_MAX;

        points.fPoints.push_back(pt);
    }

    KTINFO(testlog, "Building the kd-tree index");
    KTTreeIndex< double >* treeIndex = new KTTreeIndexEuclidean< double, KTPointCloud< Point > >(2, points, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    treeIndex->BuildIndex();

    KTINFO(testlog, "Setting up DBSCANNoiseFiltering");
    KTDBSCANNoiseFiltering filter;
    filter.SetRadius(0.1);
    filter.SetMinPoints(5);

    KTINFO(testlog, "Filtering data");
    filter.DoFiltering(treeIndex, points.fPoints);

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



