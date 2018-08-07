/*
 * KTDBSCANNoiseFiltering.cc
 *
 *  Created on: Aug 7, 2018
 *      Author: N.S. Oblath
 */

#include "KTDBSCANNoiseFiltering.hh"

#include "KTKDTreeData.hh"
#include "KTLogger.hh"
#include "KTMath.hh"

#include "KTSliceHeader.hh"
#include "KTSparseWaterfallCandidateData.hh"
#include "KTTimeFrequencyPolar.hh"
#include "KTDiscriminatedPoint.hh"

using std::set;
using std::vector;

namespace Katydid
{
    KTLOGGER(dnflog, "KTDBSCANNoiseFiltering");

    KT_REGISTER_PROCESSOR(KTDBSCANNoiseFiltering, "dbscan-noise-filtering");

    const unsigned KTDBSCANNoiseFiltering::fNDimensions = 2;

    KTDBSCANNoiseFiltering::KTDBSCANNoiseFiltering(const std::string& name) :
            KTProcessor(name),
            //fRadii(fNDimensions),
            fMinPoints(3),
            fRadius(1.),
            fCandidates(),
            fDataCount(0),
            fFilteringDoneSignal("kd-tree", this),
            fKDTreeSlot("kd-tree", this, &KTDBSCANNoiseFiltering::DoFiltering)
    {
    }

    KTDBSCANNoiseFiltering::~KTDBSCANNoiseFiltering()
    {
    }

    bool KTDBSCANNoiseFiltering::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMinPoints(node->get_value("min-points", GetMinPoints()));
        SetRadius(node->get_value("radius", GetRadius()));

        return true;
    }

    bool KTDBSCANNoiseFiltering::DoFiltering(KTKDTreeData& data)
    {
        KTPROG(dnflog, "Starting DBSCAN noise filtering");

        typedef KTDBSCAN< KTKDTreeData::TreeIndex > DBSCAN;

        DBSCAN dbscan;

        dbscan.SetRadius(fRadius);
        dbscan.SetMinPoints(fMinPoints);
        KTINFO(dnflog, "DBSCAN configured");

        for (unsigned iComponent = 0; iComponent < data.GetNComponents(); ++iComponent)
        {
            KTDEBUG(dnflog, "Clustering component " << iComponent);

            // do the clustering!
            KTINFO(dnflog, "Starting DBSCAN");
            DBSCAN::DBSResults results;
            if (! dbscan.DoClustering(*(data.GetTreeIndex(iComponent)), results))
            {
                KTERROR(dnflog, "An error occurred while clustering");
                return false;
            }
            KTDEBUG(dnflog, "DBSCAN finished");

            // TODO: everything below this in the function needs to be changed
            // loop over points in the point cloud
            //    set the noise flag from results::fNoise

            std::vector< KTKDTreeData::Point > points = data.GetSetOfPoints(iComponent);

            // loop over the clusters found, and create data objects for them
            KTDEBUG(dnflog, "Found " << results.fClusters.size() << " clusters; creating candidates");
            for (vector< DBSCAN::Cluster >::const_iterator clustIt = results.fClusters.begin(); clustIt != results.fClusters.end(); ++clustIt)
            {
                if (clustIt->empty())
                {
                    KTWARN(dnflog, "Empty cluster");
                    continue;
                }

                KTDEBUG(dnflog, "Creating candidate " << fDataCount << "; includes " << clustIt->size() << " points");

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
                cand.AddPoint(KTDiscriminatedPoint(time, freq, points[*pointIdIt].fAmplitude, timeInAcq, mean, variance, neighborhoodAmplitude));
                KTDEBUG(dnflog, "Added point #" << *pointIdIt << ": " << time << ", " << freq)

                for (++pointIdIt; pointIdIt != clustIt->end(); ++pointIdIt)
                {
                    time = points[*pointIdIt].fCoords[0] * data.GetXScaling();
                    freq = points[*pointIdIt].fCoords[1] * data.GetYScaling();
                    timeInAcq = points[*pointIdIt].fTimeInAcq * data.GetXScaling();;
                    mean = points[*pointIdIt].fMean;
                    variance = points[*pointIdIt].fVariance;
                    neighborhoodAmplitude = points[*pointIdIt].fNeighborhoodAmplitude;
                    cand.AddPoint(KTDiscriminatedPoint(time, freq, points[*pointIdIt].fAmplitude, timeInAcq, mean, variance, neighborhoodAmplitude));
                    KTDEBUG(dnflog, "Added point #" << *pointIdIt << ": " << time << ", " << freq << ", " << points[*pointIdIt].fAmplitude)

                    if (time > maxTime)
                    {
                        maxTime = time;
                    }
                    else if (time < minTime)
                    {
                        minTime = time;
                        minTimeInAcq = timeInAcq;
                        KTDEBUG(dnflog, "changing min time in Acq to time in Acq "<< minTimeInAcq)
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

        KTDEBUG(dnflog, "Clustering complete");
        fClusterDoneSignal();

        return true;
    }

} /* namespace Katydid */
