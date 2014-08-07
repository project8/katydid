/*
 * KTDBScanTrackClustering.cc
 *
 *  Created on: June 20, 2014
 *      Author: N.S. Oblath
 */

#include "KTDBScanTrackClustering.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTNOFactory.hh"
#include "KTParam.hh"
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
            KTPrimaryProcessor(name),
            fDBScan(),
            fRadii(fNDimensions),
            fMinPoints(3),
            fTimeBinWidth(1),
            fFreqBinWidth(1.),
            fCompPoints(1, DBScanPoints()),
            fCandidates(),
            fDataCount(0),
            fTrackSignal("track", this),
            fClusterDoneSignal("clustering-done", this),
            fTakePointSlot("points", this, &KTDBScanTrackClustering::TakePoints)
//            fDoClusterSlot("do-cluster-trigger", this, &KTDBScanTrackClustering::Run)
    {
        RegisterSlot("do-clustering", this, &KTDBScanTrackClustering::TriggerClustering);
        fRadii(0) = 1. / sqrt(fNDimensions);
        fRadii(1) = 1. / sqrt(fNDimensions);
    }

    KTDBScanTrackClustering::~KTDBScanTrackClustering()
    {
    }

    bool KTDBScanTrackClustering::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        SetMinPoints(node->GetValue("min-points", GetMinPoints()));

        if (node->Has("radii"))
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

        return true;
    }

    void KTDBScanTrackClustering::SetRadii(const DBScanWeights& radii)
    {
        if (radii.size() != fNDimensions)
        {
            KTERROR(tclog, "Weights vector has the wrong number of dimensions: " << radii.size() << " != " << fNDimensions);
            return;
        }
        fRadii = radii;
        return;
    }

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

        KTDBScan::Point newPoint(fNDimensions);
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

    bool KTDBScanTrackClustering::TakePoint(double time, double frequency /*, double amplitude*/, unsigned component)
    {
        if (component >= fCompPoints.size())
        {
            SetNComponents(component + 1);
        }

        KTDBScan::Point newPoint(fNDimensions);
        newPoint(0) = time;
        newPoint(1) = frequency;
        fCompPoints[component].push_back(newPoint);

        KTDEBUG(tclog, "Point " << fCompPoints[component].size()-1 << " is now " << fCompPoints[component].back());

        return true;
    }

    void KTDBScanTrackClustering::TriggerClustering()
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

    bool KTDBScanTrackClustering::DoClustering()
    {
        KTPROG(tclog, "Starting DBSCAN track clustering");

        fDBScan.SetRadius(1.);
        fDBScan.SetMinPoints(fMinPoints);
        KTINFO(tclog, "DBScan configured");

        for (unsigned iComponent = 0; iComponent < fCompPoints.size(); ++iComponent)
        //for (vector< KTDBScan >::iterator compIt = fComponents.begin(); compIt != fComponents.end(); ++compIt)
        {
            KTDEBUG(tclog, "Clustering component " << iComponent);

            if (fCompPoints[iComponent].empty() )
                continue;

            // calculate the scaling
            DBScanWeights scale = fRadii;
            for (DBScanPoint::iterator dIt = scale.begin(); dIt != scale.end(); ++dIt)
            {
                *dIt = 1. / (*dIt * KTMath::Sqrt2());
            }

            // new array for normalized points
            DBScanPoints normPoints(fCompPoints[iComponent].size());
            DBScanPoint newPoint;
            // normalize points
            KTDEBUG(tclog, "Scale: " << scale);
            unsigned iPoint = 0;
            for (DBScanPoints::iterator pIt = fCompPoints[iComponent].begin(); pIt != fCompPoints[iComponent].end(); ++pIt)
            {
//#ifndef NDEBUG
//                std::stringstream ptStr;
//                ptStr << "Point -- before: " << *pIt;
//#endif
                newPoint = element_prod(*pIt, scale);
                //KTDEBUG(tclog, ptStr.str() << " -- after: " << newPoint);
                normPoints[iPoint++] = newPoint;
            }

            // do the clustering!
            if (! fDBScan.RunDBScan< Euclidean< KTDBScan::Point > >(normPoints))
            {
                KTERROR(tclog, "An error occurred while clustering");
                return false;
            }
            KTDEBUG(tclog, "DBSCAN finished");

            // loop over the clusters found, and create data objects for them
            const vector< KTDBScan::Cluster >& clusters = fDBScan.GetClusters();
            KTDEBUG(tclog, "Found " << clusters.size() << " clusters; creating candidates");
            for (vector< KTDBScan::Cluster >::const_iterator clustIt = clusters.begin(); clustIt != clusters.end(); ++clustIt)
            {
                if (clustIt->empty())
                {
                    KTWARN(tclog, "Empty cluster");
                    continue;
                }

                KTDEBUG(tclog, "Creating candidate " << fDataCount << "; includes " << clustIt->size() << " points");

                ++fDataCount;

                KTDataPtr data(new KTData());

                KTSparseWaterfallCandidateData& cand = data->Of< KTSparseWaterfallCandidateData >();

                KTDBScan::Cluster::const_iterator pointIdIt = clustIt->begin();
                double minFreq = (fCompPoints[iComponent][*pointIdIt])(1);
                double maxFreq = minFreq;
                double minTime = (fCompPoints[iComponent][*pointIdIt])(0);
                double maxTime = minTime;
                cand.AddPoint(KTSparseWaterfallCandidateData::Point((fCompPoints[iComponent][*pointIdIt])(0), (fCompPoints[iComponent][*pointIdIt])(1), 1.));
                KTDEBUG(tclog, "Added point #" << *pointIdIt << ": " << fCompPoints[iComponent][*pointIdIt])

                for (++pointIdIt; pointIdIt != clustIt->end(); ++pointIdIt)
                {
                    cand.AddPoint(KTSparseWaterfallCandidateData::Point((fCompPoints[iComponent][*pointIdIt])(0), (fCompPoints[iComponent][*pointIdIt])(1), 1.));
                    KTDEBUG(tclog, "Added point #" << *pointIdIt << ": " << fCompPoints[iComponent][*pointIdIt])

                    if ((fCompPoints[iComponent][*pointIdIt])(1) > maxFreq)
                    {
                        maxFreq = (fCompPoints[iComponent][*pointIdIt])(1);
                    }
                    else if ((fCompPoints[iComponent][*pointIdIt])(1) < minFreq)
                    {
                        minFreq = (fCompPoints[iComponent][*pointIdIt])(1);
                    }

                    if ((fCompPoints[iComponent][*pointIdIt])(0) > maxTime)
                    {
                        maxTime = (fCompPoints[iComponent][*pointIdIt])(0);
                    }
                    else if ((fCompPoints[iComponent][*pointIdIt])(0) < minTime)
                    {
                        minTime = (fCompPoints[iComponent][*pointIdIt])(0);
                    }
                }

                cand.SetComponent(iComponent);
                cand.SetCandidateID(fDataCount);

                cand.SetTimeBinWidth(fTimeBinWidth);
                cand.SetFreqBinWidth(fFreqBinWidth);

                cand.SetTimeInRunC(minTime);
                cand.SetTimeLength(maxTime - minTime);

                cand.SetMinimumFrequency(minFreq);
                cand.SetMaximumFrequency(maxFreq);

                cand.SetFrequencyWidth(maxFreq - minFreq);

                fCandidates.insert(data);
                fTrackSignal(data);

            } // loop over clusters

            fCompPoints[iComponent].clear();

        } // loop over components

        KTDEBUG(tclog, "Clustering complete");
        fClusterDoneSignal();

        return true;
    }

    void KTDBScanTrackClustering::SetNComponents(unsigned nComps)
    {
        fCompPoints.resize(nComps, DBScanPoints());
        return;
    }

} /* namespace Katydid */
