/*
 * KTDBSCANTrackClustering.cc
 *
 *  Created on: June 20, 2014
 *      Author: N.S. Oblath
 */

#include "KTDBSCANTrackClustering.hh"
#include "KTKDTreeData.hh"
#include "logger.hh"
#include "KTMath.hh"

#include "KTSliceHeader.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTTimeFrequencyPolar.hh"
#include "KTDiscriminatedPoint.hh"

using std::set;
using std::vector;

namespace Katydid
{
    LOGGER(tclog, "katydid.fft");

    KT_REGISTER_PROCESSOR(KTDBSCANTrackClustering, "dbscan-track-clustering");

    const unsigned KTDBSCANTrackClustering::fNDimensions = 2;

    KTDBSCANTrackClustering::KTDBSCANTrackClustering(const std::string& name) :
            KTProcessor(name),
            //fRadii(fNDimensions),
            fMinPoints(3),
            fRadius(1.),
            //fTimeBinWidth(1),
            //fFreqBinWidth(1.),
            //fCompPoints(1, Points()),
            fCandidates(),
            fDataCount(0),
            fTrackSignal("track", this),
            fClusterDoneSignal("clustering-done", this),
            fClusterKDTreeSlot("kd-tree", this, &KTDBSCANTrackClustering::DoClustering)
            //fTakePointSlot("points", this, &KTDBSCANTrackClustering::TakePoints)
//            fDoClusterSlot("do-cluster-trigger", this, &KTDBSCANTrackClustering::Run)
    {
        //RegisterSlot("do-clustering", this, &KTDBSCANTrackClustering::DoClusteringSlot);
        //fRadii(0) = 1. / sqrt(fNDimensions);
        //fRadii(1) = 1. / sqrt(fNDimensions);
    }

    KTDBSCANTrackClustering::~KTDBSCANTrackClustering()
    {
    }

    bool KTDBSCANTrackClustering::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMinPoints(node->get_value("min-points", GetMinPoints()));
        SetRadius(node->get_value("radius", GetRadius()));

        /*
        if (node->has("radii"))
        {
            const KTParamArray* radii = node->ArrayAt("radii");
            if (radii->Size() != fNDimensions)
            {
                LERROR(tclog, "Radii array does not have the right number of dimensions: <" << radii->Size() << "> instead of <" << fNDimensions << ">");
                return false;
            }
            fRadii(0) = radii->GetValue< double >(0);
            fRadii(1) = radii->GetValue< double >(1);
        }
        */

        return true;
    }

    /*
    bool KTDBSCANTrackClustering::TakePoints(KTSliceHeader& slHeader, KTDiscriminatedPoints1DData& discPoints)
    {
        // first check to see if this is a new acquisition; if so, run clustering on the previous acquistion's data
        if (slHeader.GetIsNewAcquisition())
        {
            if (! DoClustering())
            {
                LERROR(tclog, "An error occurred while clustering from the previous acquisition");
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
                LDEBUG(tclog, "Point " << fCompPoints[iComponent].size()-1 << " is now " << fCompPoints[iComponent].back());
            }
        }

        return true;
    }

    bool KTDBSCANTrackClustering::TakePoint(double time, double frequency *//*, double amplitude*//*, unsigned component)
    {
        if (component >= fCompPoints.size())
        {
            SetNComponents(component + 1);
        }

        Point newPoint(fNDimensions);
        newPoint(0) = time;
        newPoint(1) = frequency;
        fCompPoints[component].push_back(newPoint);

        LDEBUG(tclog, "Point " << fCompPoints[component].size()-1 << " is now " << fCompPoints[component].back());

        return true;
    }

    void KTDBSCANTrackClustering::DoClusteringSlot()
    {
        if (! Run())
        {
            LERROR(tclog, "An error occurred while running the track clustering");
        }
        return;
    }

    bool KTDBSCANTrackClustering::Run()
    {
        return DoClustering();
    }
    */


    bool KTDBSCANTrackClustering::DoClustering(KTKDTreeData& data)
    {
        KTPROG(tclog, "Starting DBSCAN track clustering");

        typedef KTDBSCAN< KTKDTreeData::TreeIndex > DBSCAN;

        DBSCAN dbScan;

        dbScan.SetRadius(fRadius);
        dbScan.SetMinPoints(fMinPoints);
        LINFO(tclog, "DBSCAN configured");

        for (unsigned iComponent = 0; iComponent < data.GetNComponents(); ++iComponent)
        {
            LDEBUG(tclog, "Clustering component " << iComponent);

            // do the clustering!
            LINFO(tclog, "Starting DBSCAN");
            DBSCAN::DBSResults results;
            if (! dbScan.DoClustering(*(data.GetTreeIndex(iComponent)), results))
            {
                LERROR(tclog, "An error occurred while clustering");
                return false;
            }
            LDEBUG(tclog, "DBSCAN finished");

            std::vector< KTKDTreeData::Point > points = data.GetSetOfPoints(iComponent);

            // loop over the clusters found, and create data objects for them
            LDEBUG(tclog, "Found " << results.fClusters.size() << " clusters; creating candidates");
            for (vector< DBSCAN::Cluster >::const_iterator clustIt = results.fClusters.begin(); clustIt != results.fClusters.end(); ++clustIt)
            {
                if (clustIt->empty())
                {
                    LWARN(tclog, "Empty cluster");
                    continue;
                }

                LDEBUG(tclog, "Creating candidate " << fDataCount << "; includes " << clustIt->size() << " points");

                ++fDataCount;

                Nymph::KTDataPtr newData(new Nymph::KTData());
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
                double mean = points[*pointIdIt].fMean;
                double variance = points[*pointIdIt].fVariance;
                double neighborhoodAmplitude = points[*pointIdIt].fNeighborhoodAmplitude;
                cand.AddPoint(KTDiscriminatedPoint(time, freq, points[*pointIdIt].fAmplitude, timeInAcq, mean, variance, neighborhoodAmplitude, points[*pointIdIt].fBinInSlice));
                LDEBUG(tclog, "Added point #" << *pointIdIt << ": " << time << ", " << freq)

                for (++pointIdIt; pointIdIt != clustIt->end(); ++pointIdIt)
                {
                    time = points[*pointIdIt].fCoords[0] * data.GetXScaling();
                    freq = points[*pointIdIt].fCoords[1] * data.GetYScaling();
                    timeInAcq = points[*pointIdIt].fTimeInAcq * data.GetXScaling();;
                    mean = points[*pointIdIt].fMean;
                    variance = points[*pointIdIt].fVariance;
                    neighborhoodAmplitude = points[*pointIdIt].fNeighborhoodAmplitude;
                    cand.AddPoint(KTDiscriminatedPoint(time, freq, points[*pointIdIt].fAmplitude, timeInAcq, mean, variance, neighborhoodAmplitude, points[*pointIdIt].fBinInSlice));
                    LDEBUG(tclog, "Added point #" << *pointIdIt << ": " << time << ", " << freq << ", " << points[*pointIdIt].fAmplitude)

                    if (time > maxTime)
                    {
                        maxTime = time;
                    }
                    else if (time < minTime)
                    {
                        minTime = time;
                        minTimeInAcq = timeInAcq;
                        LDEBUG(tclog, "changing min time in Acq to time in Acq "<< minTimeInAcq)
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

                cand.SetMinFrequency(minFreq);
                cand.SetMaxFrequency(maxFreq);

                cand.SetFrequencyWidth(maxFreq - minFreq);

                fCandidates.insert(newData);
                fTrackSignal(newData);

            } // loop over clusters

        } // loop over components

        LDEBUG(tclog, "Clustering complete");
        fClusterDoneSignal();

        return true;
    }

} /* namespace Katydid */
