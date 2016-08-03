/*
 * KTDBScanTrackClustering.cc
 *
 *  Created on: June 20, 2014
 *      Author: N.S. Oblath
 */

#include "KTDBScanTrackClustering.hh"

#include "KTKDTreeData.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTNOFactory.hh"
#include "KTSliceHeader.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTTimeFrequencyPolar.hh"

using std::set;
using std::vector;

namespace Katydid
{
    KTLOGGER(tclog, "katydid.fft");

    KT_REGISTER_PROCESSOR(KTDBScanTrackClustering, "dbscan-track-clustering");

    const unsigned KTDBScanTrackClustering::fNDimensions = 2;

    KTDBScanTrackClustering::KTDBScanTrackClustering(const std::string& name) :
            KTProcessor(name),
            //fRadii(fNDimensions),
            fMinPoints(3),
            //fTimeBinWidth(1),
            //fFreqBinWidth(1.),
            //fCompPoints(1, Points()),
            fCandidates(),
            fDataCount(0),
            fTrackSignal("track", this),
            fClusterDoneSignal("clustering-done", this),
            fClusterKDTreeSlot("kd-tree", this, &KTDBScanTrackClustering::DoClustering)
            //fTakePointSlot("points", this, &KTDBScanTrackClustering::TakePoints)
//            fDoClusterSlot("do-cluster-trigger", this, &KTDBScanTrackClustering::Run)
    {
        //RegisterSlot("do-clustering", this, &KTDBScanTrackClustering::DoClusteringSlot);
        //fRadii(0) = 1. / sqrt(fNDimensions);
        //fRadii(1) = 1. / sqrt(fNDimensions);
    }

    KTDBScanTrackClustering::~KTDBScanTrackClustering()
    {
    }

    bool KTDBScanTrackClustering::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMinPoints(node->get_value("min-points", GetMinPoints()));

        /*
        if (node->has("radii"))
        {
            const KTParamArray* radii = node->ArrayAt("radii");
            if (radii->Size() != fNDimensions)
            {
                KTERROR(tclog, "Radii array does not have the right number of dimensions: <" << radii->Size() << "> instead of <" << fNDimensions << ">");
                return false;
            }
            fRadii(0) = radii->GetValue< double >(0);
            fRadii(1) = radii->GetValue< double >(1);
        }
        */

        return true;
    }

    /*
    bool KTDBScanTrackClustering::TakePoints(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints)
    {
        // first check to see if this is a new acquisition; if so, run clustering on the previous acquistion's data
        if (slHeader.GetIsNewAcquisition())
        {
            if (! DoClustering())
            {
                KTERROR(tclog, "An error occurred while clustering from the previous acquisition");
                return false;
            }
        }

        // update time and frequency bin widths, though they probably don't actually need to be updated
        fTimeBinWidth = slHeader.GetSliceLength();
        fFreqBinWidth = discPoints.GetBinWidth();

        // verify that we have the right number of components
        if (slHeader.GetNComponents() > fCompPoints.size())
        {
            SetNComponents(slHeader.GetNComponents());
        }

        Point newPoint(fNDimensions);
        newPoint(0) = slHeader.GetTimeInRun() + 0.5 * fTimeBinWidth;
        for (unsigned iComponent = 0; iComponent != fCompPoints.size(); ++iComponent)
        {
            const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discPoints.GetSetOfPoints(iComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin();
                    pIt != incomingPts.end(); ++pIt)
            {
                newPoint(1) = pIt->second.fAbscissa;
                fCompPoints[iComponent].push_back(newPoint);
                KTDEBUG(tclog, "Point " << fCompPoints[iComponent].size()-1 << " is now " << fCompPoints[iComponent].back());
            }
        }

        return true;
    }

    bool KTDBScanTrackClustering::TakePoint(double time, double frequency *//*, double amplitude*//*, unsigned component)
    {
        if (component >= fCompPoints.size())
        {
            SetNComponents(component + 1);
        }

        Point newPoint(fNDimensions);
        newPoint(0) = time;
        newPoint(1) = frequency;
        fCompPoints[component].push_back(newPoint);

        KTDEBUG(tclog, "Point " << fCompPoints[component].size()-1 << " is now " << fCompPoints[component].back());

        return true;
    }

    void KTDBScanTrackClustering::DoClusteringSlot()
    {
        if (! Run())
        {
            KTERROR(tclog, "An error occurred while running the track clustering");
        }
        return;
    }

    bool KTDBScanTrackClustering::Run()
    {
        return DoClustering();
    }
    */


    bool KTDBScanTrackClustering::DoClustering(KTKDTreeData& data)
    {
        KTPROG(tclog, "Starting DBSCAN track clustering");

        typedef KTDBScan< KTKDTreeData::TreeIndex > DBSCAN;

        DBSCAN dbScan;

        dbScan.SetRadius(1.);
        dbScan.SetMinPoints(fMinPoints);
        KTINFO(tclog, "DBScan configured");

        for (unsigned iComponent = 0; iComponent < data.GetNComponents(); ++iComponent)
        {
            KTDEBUG(tclog, "Clustering component " << iComponent);

            // do the clustering!
            KTINFO(tclog, "Starting DBSCAN");
            DBSCAN::DBSResults results;
            if (! dbScan.DoClustering(*(data.GetTreeIndex(iComponent)), results))
            {
                KTERROR(tclog, "An error occurred while clustering");
                return false;
            }
            KTDEBUG(tclog, "DBSCAN finished");

            std::vector< KTKDTreeData::Point > points = data.GetSetOfPoints(iComponent);

            // loop over the clusters found, and create data objects for them
            KTDEBUG(tclog, "Found " << results.fClusters.size() << " clusters; creating candidates");
            for (vector< DBSCAN::Cluster >::const_iterator clustIt = results.fClusters.begin(); clustIt != results.fClusters.end(); ++clustIt)
            {
                if (clustIt->empty())
                {
                    KTWARN(tclog, "Empty cluster");
                    continue;
                }

                KTDEBUG(tclog, "Creating candidate " << fDataCount << "; includes " << clustIt->size() << " points");

                ++fDataCount;

                KTDataPtr newData(new KTData());
                KTSparseWaterfallCandidateData& cand = newData->Of< KTSparseWaterfallCandidateData >();

                DBSCAN::Cluster::const_iterator pointIdIt = clustIt->begin();
                double time = points[*pointIdIt].fCoords[0] * data.GetXScaling();
                double freq = points[*pointIdIt].fCoords[1] * data.GetYScaling();
                double timeInAcq = points[*pointIdIt].fTimeInAcq * data.GetXScaling();
                double minFreq = freq;
                double maxFreq = minFreq;
                double minTime = time;
                double minTimeInAcq = timeInAcq;
                double maxTime = minTime;
                cand.AddPoint(KTSparseWaterfallCandidateData::Point(time, freq, 1., timeInAcq));
                KTDEBUG(tclog, "Added point #" << *pointIdIt << ": " << time << ", " << freq)

                for (++pointIdIt; pointIdIt != clustIt->end(); ++pointIdIt)
                {
                    time = points[*pointIdIt].fCoords[0] * data.GetXScaling();
                    freq = points[*pointIdIt].fCoords[1] * data.GetYScaling();
                    timeInAcq = points[*pointIdIt].fTimeInAcq * data.GetXScaling();;
                    cand.AddPoint(KTSparseWaterfallCandidateData::Point(time, freq, 1., timeInAcq));
                    KTDEBUG(tclog, "Added point #" << *pointIdIt << ": " << time << ", " << freq)

                    if (time > maxTime)
                    {
                        maxTime = time;
                    }
                    else if (time < minTime)
                    {
                        minTime = time;
                        minTimeInAcq = timeInAcq;
                    }

                    if (freq > maxFreq)
                    {
                        maxFreq = freq;
                    }
                    else if (freq < minFreq)
                    {
                        minFreq = freq;
                    }
                }

                cand.SetComponent(iComponent);
                cand.SetAcquisitionID(data.GetAcquisitionID());
                cand.SetCandidateID(fDataCount);

                //cand.SetTimeBinWidth(fTimeBinWidth);
                //cand.SetFreqBinWidth(fFreqBinWidth);

                cand.SetTimeInRunC(minTime);
                cand.SetTimeInAcq(minTimeInAcq);
                cand.SetTimeLength(maxTime - minTime);

                cand.SetMinimumFrequency(minFreq);
                cand.SetMaximumFrequency(maxFreq);

                cand.SetFrequencyWidth(maxFreq - minFreq);

                fCandidates.insert(newData);
                fTrackSignal(newData);

            } // loop over clusters

        } // loop over components

        KTDEBUG(tclog, "Clustering complete");
        fClusterDoneSignal();

        return true;
    }

} /* namespace Katydid */
