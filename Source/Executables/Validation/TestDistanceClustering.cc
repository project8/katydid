/*
 * TestDistanceClustering.cc
 *
 *  Created on: Dec 18, 2012
 *      Author: nsoblath
 */

#include "KTCluster1DData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTDistanceClustering.hh"
#include "KTLogger.hh"


using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "TestDistanceClustering");

int main()
{
    KTINFO(testlog, "Setting up points");

    KTDiscriminatedPoints1DData discPoints;
    discPoints.SetNComponents(1);
    discPoints.SetBinWidth(1.);
    discPoints.SetNBins(100);

    discPoints.AddPoint(8, KTDiscriminatedPoints1DData::Point(0.7, 1.0, 0.1), 0);
    discPoints.AddPoint(27, KTDiscriminatedPoints1DData::Point(0.9, 1.0, 0.1), 0);
    discPoints.AddPoint(13, KTDiscriminatedPoints1DData::Point(0.4, 1.0, 0.1), 0);
    discPoints.AddPoint(14, KTDiscriminatedPoints1DData::Point(0.88, 1.0, 0.1), 0);
    discPoints.AddPoint(15, KTDiscriminatedPoints1DData::Point(0.3, 1.0, 0.1), 0);
    discPoints.AddPoint(92, KTDiscriminatedPoints1DData::Point(0.2, 1.0, 0.1), 0);
    discPoints.AddPoint(90, KTDiscriminatedPoints1DData::Point(0.9, 1.0, 0.1), 0);
    discPoints.AddPoint(87, KTDiscriminatedPoints1DData::Point(0.5, 1.0, 0.1), 0);

    unsigned iPoint = 0;
    for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt=discPoints.GetSetOfPoints(0).begin(); pIt != discPoints.GetSetOfPoints(0).end(); pIt++)
    {
        KTINFO(testlog, "Point #" << iPoint << ":  " << pIt->first << "  " << pIt->second.fAbscissa);
        iPoint++;
    }

    KTDistanceClustering clustering;
    clustering.SetMaxBinDistance(2);

    KTINFO(testlog, "Analyzing points");
    if (! clustering.FindClusters(discPoints))
    {
        KTERROR(testlog, "Something went wrong while finding clusters");
        return -1;
    }
    KTCluster1DData& clusters = discPoints.Of< KTCluster1DData >();

    unsigned iCluster = 0;
    for (KTCluster1DData::SetOfClusters::const_iterator cIt = clusters.GetSetOfClusters(0).begin(); cIt != clusters.GetSetOfClusters(0).end(); cIt++)
    {
        KTINFO(testlog, "Cluster #" << iCluster << ":  " << cIt->first << " --> " << cIt->second);
        iCluster++;
    }

    return 0;
}

