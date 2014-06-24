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
#include "KTTimeFrequencyPolar.hh"

using std::vector;

namespace Katydid
{
    KTLOGGER(tclog, "katydid.fft");

    KT_REGISTER_PROCESSOR(KTDBScanTrackClustering, "dbscan-track-clustering");

    KTDBScanTrackClustering::KTDBScanTrackClustering(const std::string& name) :
            KTPrimaryProcessor(name),
            fEpsilon(1.),
            fMinPoints(3),
            fComponents(1, KTDBScan(fEpsilon, fMinPoints)),
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
            fComponents[iComponent].ComputeSimilarity(dist);
            if (! fComponents[iComponent].DoClustering() )
            {
                KTERROR(tclog, "An error occurred while clustering");
                return false;
            }

            const vector< KTDBScan::Cluster >& clusters = fComponents[iComponent].GetClusters();
            for (vector< KTDBScan::Cluster >::const_iterator clustIt = clusters.begin(); clustIt != clusters.end(); ++clustIt)
            {
                if (clustIt->empty())
                {
                    continue;
                }

                fCandidates.push_back(KTWaterfallCandidateData());
                KTWaterfallCandidateData& cand = fCandidates.back();

                KTDBScan::Cluster::const_iterator pointIt = clustIt->begin();
                double minFreq = (*pointIt)[1];
                double maxFreq = minFreq;
                double minTime = (*pointIt)[0];
                double maxTime = minTime;

                for (++pointIt; pointIt != clustIt->end(); ++pointIt)
                {
                    if ((*pointIt)[1] > maxFreq)
                    {
                        maxFreq = (*pointIt)[1];
                    }
                    else if ((*pointIt)[1] < minFreq)
                    {
                        minFreq = (*pointIt)[1];
                    }

                    if ((*pointIt)[0] > maxTime)
                    {
                        maxTime = (*pointIt)[0];
                    }
                    else if ((*pointIt)[0] < minTime)
                    {
                        minTime = (*pointIt)[0];
                    }
                }

                KTTimeFrequencyPolar* tf = new KTTimeFrequencyPolar();

                cand.SetComponent(iComponent);
            }

            /*
            KTTimeFrequency* GetCandidate() const;

            unsigned GetNTimeBins() const;
            double GetTimeBinWidth() const;

            unsigned GetNFreqBins() const;
            double GetFreqBinWidth() const;

            double GetTimeInRun() const;
            double GetTimeLength() const;
            uint64_t GetFirstSliceNumber() const;
            uint64_t GetLastSliceNumber() const;
            double GetMinimumFrequency() const;
            double GetMaximumFrequency() const;
            double GetMeanStartFrequency() const;
            double GetMeanEndFrequency() const;
            double GetFrequencyWidth() const;

            unsigned GetStartRecordNumber() const;
            unsigned GetStartSampleNumber() const;
            unsigned GetEndRecordNumber() const;
            unsigned GetEndSampleNumber() const;
             *
             */


        }
    }


} /* namespace Katydid */
