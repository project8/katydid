/*
 * KTDBScanTrackClustering.cc
 *
 *  Created on: June 20, 2014
 *      Author: N.S. Oblath
 */

#include "KTDBScanTrackClustering.hh"

#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTPStoreNode.hh"



namespace Katydid
{
    KTLOGGER(tclog, "katydid.fft");

    KT_REGISTER_PROCESSOR(KTDBScanTrackClustering, "dbscan-track-clustering");

    KTDBScanTrackClustering::KTDBScanTrackClustering(const std::string& name) :
            KTPrimaryProcessor(name),
            fEpsilon(1.),
            fMinPoints(3),
            fNoise(),
            fVisited(),
            fPoints(),
            fPointIdToClusterId(),
            fClusters(),
            fSim(),
            fTrackSignal("track", this),
            fTakePointSlot("point", this, &KTDBScanTrackClustering::TakePoint)
    {
    }

    KTDBScanTrackClustering::~KTDBScanTrackClustering()
    {
    }

    bool KTDBScanTrackClustering::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return false;

        fEpsilon = node->GetData("epsilon", fEpsilon);
        fMinPoints = node->GetData("min-points", fMinPoints);

        return true;
    }

    bool KTDBScanTrackClustering::TakePoint(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints)
    {

    }

    bool KTDBScanTrackClustering::TakePoints(const Points& points)
    {
        fPoints.push_back(points.begin(), points.end());
        return true;
    }

    bool KTDBScanTrackClustering::TakePoint(const Point& point)
    {
        fPoints.push_back(point);
        return true;
    }

    void KTDBScanTrackClustering::ResetPoints()
    {
        fPoints.clear();
        return;
    }

    bool KTDBScanTrackClustering::Run()
    {

    }

    void KTDBScanTrackClustering::UniformPartition()
    {

    }

    KTDBScanTrackClustering::Neighbors KTDBScanTrackClustering::FindNeighbors(PointId pid, double threshold)
    {

    }

    bool KTDBScanTrackClustering::DoClustering()
    {

    }


} /* namespace Katydid */
