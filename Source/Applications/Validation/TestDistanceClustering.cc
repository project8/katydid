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

KTLOGGER(testlog, "katydid.applications.validation");

int main()
{
    KTINFO(testlog, "Setting up points");

    KTDiscriminatedPoints1DData discPoints(1);
    discPoints.SetBinWidth(1.);
    discPoints.SetNBins(100);
    discPoints.SetThreshold(0.1, 0);

    discPoints.AddPoint(8, 0.7, 0);
    discPoints.AddPoint(27, 0.9, 0);
    discPoints.AddPoint(13, 0.4, 0);
    discPoints.AddPoint(14, 0.88, 0);
    discPoints.AddPoint(15, 0.3, 0);
    discPoints.AddPoint(92, 0.2, 0);
    discPoints.AddPoint(90, 0.9, 0);
    discPoints.AddPoint(87, 0.5, 0);

    UInt_t iPoint = 0;
    for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt=discPoints.GetSetOfPoints(0).begin(); pIt != discPoints.GetSetOfPoints(0).end(); pIt++)
    {
        KTINFO(testlog, "Point #" << iPoint << ":  " << pIt->first << "  " << pIt->second);
        iPoint++;
    }

    KTDistanceClustering clustering;
    clustering.SetMaxBinDistance(2);

    KTINFO(testlog, "Analyzing points");
    KTCluster1DData* clusters = clustering.FindClusters(&discPoints);

    UInt_t iCluster = 0;
    for (KTCluster1DData::SetOfClusters::const_iterator cIt = clusters->GetSetOfClusters(0).begin(); cIt != clusters->GetSetOfClusters(0).end(); cIt++)
    {
        KTINFO(testlog, "Cluster #" << iCluster << ":  " << cIt->first << " --> " << cIt->second);
        iCluster++;
    }

    delete clusters;

    return 0;
}

