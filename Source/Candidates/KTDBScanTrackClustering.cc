/*
 * KTDBScanTrackClustering.cc
 *
 *  Created on: June 20, 2014
 *      Author: N.S. Oblath
 */

#include "KTDBScanTrackClustering.hh"

#include "KTDiscriminatedPoints1DData.hh"
#include "KTLogger.hh"
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

    KTDBScanTrackClustering::KTDBScanTrackClustering(const std::string& name) :
            KTPrimaryProcessor(name),
            fEpsilon(1.),
            fMinPoints(3),
            fTimeBinWidth(0.),
            fFreqBinWidth(0.),
            fComponents(1, KTDBScan(fEpsilon, fMinPoints)),
            fCandidates(),
            fDataCount(0),
            fTrackSignal("track", this),
            fTakePointSlot("points", this, &KTDBScanTrackClustering::TakePoints)
    {
    }

    KTDBScanTrackClustering::~KTDBScanTrackClustering()
    {
    }

    bool KTDBScanTrackClustering::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        SetEpsilon(node->GetValue("epsilon", fEpsilon));
        SetMinPoints(node->GetValue("min-points", fMinPoints));

        return true;
    }

    void KTDBScanTrackClustering::UpdateComponents()
    {
        // update the DBScan components
        for (vector< KTDBScan >::iterator compIt = fComponents.begin(); compIt != fComponents.end(); ++compIt)
        {
            compIt->SetEpsilon(fEpsilon);
            compIt->SetMinPoints(fMinPoints);
        }
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
        fTimeBinWidth = slHeader.GetBinWidth();
        fFreqBinWidth = discPoints.GetBinWidth();

        // verify that we have the right number of components
        if (slHeader.GetNComponents() > fComponents.size())
        {
            fComponents.resize(slHeader.GetNComponents(), KTDBScan(fEpsilon, fMinPoints));
        }

        KTDBScan::Point newPoint(2);
        newPoint[0] = slHeader.GetTimeInRun();
        for (unsigned iComponent = 0; iComponent != fComponents.size(); ++iComponent)
        {
            const KTDiscriminatedPoints1DData::SetOfPoints&  incomingPts = discPoints.GetSetOfPoints(iComponent);
            for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator pIt = incomingPts.begin(); pIt != incomingPts.end(); ++pIt)
            {
                newPoint[1] = pIt->second.fAbscissa;
                fComponents[iComponent].TakePoint(newPoint);
            }
        }

        return true;
    }

    bool KTDBScanTrackClustering::Run()
    {
        return DoClustering();
    }

    bool KTDBScanTrackClustering::DoClustering()
    {
        Distance< Cosine< KTDBScan::Point > > dist;
        for (unsigned iComponent = 0; iComponent < fComponents.size(); ++iComponent)
        //for (vector< KTDBScan >::iterator compIt = fComponents.begin(); compIt != fComponents.end(); ++compIt)
        {
            // do the clustering!
            fComponents[iComponent].ComputeSimilarity(dist);
            if (! fComponents[iComponent].DoClustering() )
            {
                KTERROR(tclog, "An error occurred while clustering");
                return false;
            }

            // loop over the clusters found, and create data objects for them
            const vector< KTDBScan::Cluster >& clusters = fComponents[iComponent].GetClusters();
            const KTDBScan::Points& points = fComponents[iComponent].GetPoints();
            for (vector< KTDBScan::Cluster >::const_iterator clustIt = clusters.begin(); clustIt != clusters.end(); ++clustIt)
            {
                if (clustIt->empty())
                {
                    continue;
                }

                ++fDataCount;

                KTDataPtr data(new KTData());

                KTSparseWaterfallCandidateData& cand = data->Of< KTSparseWaterfallCandidateData >();

                KTDBScan::Cluster::const_iterator pointIdIt = clustIt->begin();
                double minFreq = (points[*pointIdIt])[1];
                double maxFreq = minFreq;
                double minTime = (points[*pointIdIt])[0];
                double maxTime = minTime;

                for (++pointIdIt; pointIdIt != clustIt->end(); ++pointIdIt)
                {
                    cand.AddPoint(KTSparseWaterfallCandidateData::Point((points[*pointIdIt])[0], (points[*pointIdIt])[1], 1.));

                    if ((points[*pointIdIt])[1] > maxFreq)
                    {
                        maxFreq = (points[*pointIdIt])[1];
                    }
                    else if ((points[*pointIdIt])[1] < minFreq)
                    {
                        minFreq = (points[*pointIdIt])[1];
                    }

                    if ((points[*pointIdIt])[0] > maxTime)
                    {
                        maxTime = (points[*pointIdIt])[0];
                    }
                    else if ((points[*pointIdIt])[0] < minTime)
                    {
                        minTime = (points[*pointIdIt])[0];
                    }
                }

                cand.SetComponent(iComponent);

                cand.SetTimeBinWidth(fTimeBinWidth);
                cand.SetFreqBinWidth(fFreqBinWidth);

                cand.SetTimeInRun(minTime);
                cand.SetTimeLength(maxTime - minTime);

                cand.SetMinimumFrequency(minFreq);
                cand.SetMaximumFrequency(maxFreq);

                cand.SetFrequencyWidth(maxFreq - minFreq);

                fCandidates.insert(data);
                fTrackSignal(data);

            } // loop over clusters

        } // loop over components

        return true;
    }


} /* namespace Katydid */
