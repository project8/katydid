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
    std::pair< double, double > trackYRange( 0.6, 0.605 );
    unsigned nTrackPoints = 100;

    KTINFO(testlog, "Creating data");

    KTPointCloud< KTKDTreeData::Point > points;
    points.fPoints.reserve(nNoisePoints + nTrackPoints);

    double delta = range.second - range.first;
    for (unsigned iPoint = 0; iPoint < nNoisePoints; ++iPoint)
    {
        KTKDTreeData::Point pt;

        pt.fCoords[0] = range.first + rand() * delta / RAND_MAX;
        pt.fCoords[1] = range.first + rand() * delta / RAND_MAX;

        points.fPoints.push_back(pt);
    }

    double deltaX = trackXRange.second - trackXRange.first;
    double deltaY = trackYRange.second - trackYRange.first;
    for (unsigned iPoint = 0; iPoint < nTrackPoints; ++iPoint)
    {
        KTKDTreeData::Point pt;

        pt.fCoords[0] = trackXRange.first + rand() * deltaX / RAND_MAX;
        pt.fCoords[1] = trackYRange.first + rand() * deltaY / RAND_MAX;

        points.fPoints.push_back(pt);
    }

    KTINFO(testlog, "Building the kd-tree index");
    KTTreeIndex< double >* treeIndex = new KTTreeIndexEuclidean< double, KTPointCloud< KTKDTreeData::Point > >(2, points, nanoflann::KDTreeSingleIndexAdaptorParams(10));
    treeIndex->BuildIndex();

    KTINFO(testlog, "Setting up DBSCANNoiseFiltering");
    KTDBSCANNoiseFiltering filter;
    filter.SetRadius(0.01);
    filter.SetMinPoints(3);

    KTINFO(testlog, "Filtering data");
    filter.DoFiltering(treeIndex, points.fPoints);

    //KTINFO(testlog, results);





#ifdef ROOT_FOUND
    TFile file("dbscan_noise_filter_test.root", "recreate");
    TCanvas* canv = new TCanvas("cNoiseFilterTest", "Noise Filter Test");

    TGraph* ptsGraph = new TGraph(points.kdtree_get_point_count());
    ptsGraph->SetMarkerStyle(1);
    ptsGraph->SetMarkerColor(1);
    unsigned nNoisePointsFound = 0;
    for (unsigned pid = 0; pid < points.kdtree_get_point_count(); ++pid)
    {
        ptsGraph->SetPoint(pid, points.kdtree_get_pt(pid, 0), points.kdtree_get_pt(pid, 1));
        if (points.fPoints.at(pid).fNoiseFlag) ++nNoisePointsFound;
    }
    ptsGraph->Draw("ap");
    ptsGraph->Write("Points");

    KTINFO(testlog, "Number of noise points: " << nNoisePointsFound);

    TGraph* noiseGraph = new TGraph(nNoisePoints);
    noiseGraph->SetMarkerStyle(4);
    noiseGraph->SetMarkerColor(2);
    for (unsigned pid = 0; pid < points.kdtree_get_point_count(); ++pid)
    {
        if (points.fPoints.at(pid).fNoiseFlag)
        {
            noiseGraph->SetPoint(pid, points.kdtree_get_pt(pid, 0), points.kdtree_get_pt(pid, 1));
        }
    }
    noiseGraph->Draw("psame");
    noiseGraph->Write("Noise");

    canv->Write();

    file.Close();
#endif


    return 0;
}



