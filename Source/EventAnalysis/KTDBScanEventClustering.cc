/*
 * KTDBScanEventClustering.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: N.S. Oblath
 */

#include "KTDBScanEventClustering.hh"

#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTMultiTrackEventData.hh"
#include "KTNOFactory.hh"
#include "KTParam.hh"
#include "KTProcessedTrackData.hh"

#ifndef NDEBUG
#include <sstream>
#endif

using std::set;
using std::vector;

namespace Katydid
{
    KTLOGGER(tclog, "katydid.fft");

    KT_REGISTER_PROCESSOR(KTDBScanEventClustering, "dbscan-event-clustering");

    // dimensions: (t_start, f_start, t_end, f_end)
    const unsigned KTDBScanEventClustering::fNDimensions = 4;
    // points in a track: (start, end)
    const unsigned KTDBScanEventClustering::fNPointsPerTrack = 2;

    KTDBScanEventClustering::KTDBScanEventClustering(const std::string& name) :
            KTPrimaryProcessor(name),
            fRadii(fNDimensions / fNPointsPerTrack),
            fMinPoints(3),
            fTimeBinWidth(1),
            fFreqBinWidth(1.),
            fCompTracks(1, vector< KTProcessedTrackData >()),
            fCandidates(),
            fDataCount(0),
            fEventSignal("event", this),
            fClusterDoneSignal("clustering-done", this),
            fTakeTrackSlot("track", this, &KTDBScanEventClustering::TakeTrack)
    //        fDoClusterSlot("do-cluster-trigger", this, &KTDBScanEventClustering::Run)
    {
        RegisterSlot("do-clustering", this, &KTDBScanEventClustering::DoClusteringSlot);
        fRadii(0) = 1. / sqrt(fNDimensions);
        fRadii(1) = 1. / sqrt(fNDimensions);
    }

    KTDBScanEventClustering::~KTDBScanEventClustering()
    {
    }

    bool KTDBScanEventClustering::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        SetMinPoints(node->GetValue("min-points", GetMinPoints()));

        if (node->Has("radii"))
        {
            const KTParamArray* radii = node->ArrayAt("radii");
            if (radii->Size() != fNDimensions / fNPointsPerTrack)
            {
                KTERROR(tclog, "Radii array does not have the right number of dimensions: <" << radii->Size() << "> instead of <" << fNDimensions/fNPointsPerTrack << ">");
                return false;
            }
            fRadii(0) = radii->GetValue< double >(0);
            fRadii(1) = radii->GetValue< double >(1);
        }

        return true;
    }

    bool KTDBScanEventClustering::TakeTrack(KTProcessedTrackData& track)
    {
        // ignore the track if it's been cut
        if (track.GetIsCut()) return true;

        // verify that we have the right number of components
        if (track.GetComponent() >= fCompTracks.size())
        {
            SetNComponents(track.GetComponent() + 1);
        }

        // copy the full track data
        fCompTracks[track.GetComponent()].push_back(track);

        KTDEBUG(tclog, "Taking track: (" << track.GetStartTimeInRunC() << ", " << track.GetStartFrequency() << ", " << track.GetEndTimeInRunC() << ", " << track.GetEndFrequency());

        return true;
    }

    /*
    bool KTDBScanEventClustering::TakePoint(double time, double frequency *//*, double amplitude*//*, unsigned component)
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
    */

    void KTDBScanEventClustering::DoClusteringSlot()
    {
        if (! Run())
        {
            KTERROR(tclog, "An error occurred while running the event clustering");
        }
        return;
    }

    bool KTDBScanEventClustering::Run()
    {
        return DoClustering();
    }

    bool KTDBScanEventClustering::DoClustering()
    {
        KTPROG(tclog, "Starting DBSCAN event clustering");

        KTDBScan< DistanceMatrix > dbScan;

        dbScan.SetRadius(1.);
        dbScan.SetMinPoints(fMinPoints);
        KTINFO(tclog, "DBScan configured");

        for (unsigned iComponent = 0; iComponent < fCompTracks.size(); ++iComponent)
        {
            KTDEBUG(tclog, "Clustering component " << iComponent);

            if (fCompTracks[iComponent].empty() )
                continue;

            // calculate the scaling
            Point scale = fRadii;
            for (Point::iterator dIt = scale.begin(); dIt != scale.end(); ++dIt)
            {
                *dIt = 1. / (*dIt * KTMath::Sqrt2());
            }

            // new array for normalized points
            Points normPoints(fCompTracks[iComponent].size());
            Point newPoint(fNDimensions);
            // normalize points
            KTDEBUG(tclog, "Scale: " << scale);
            unsigned iPoint = 0;
            for (vector< KTProcessedTrackData >::const_iterator pIt = fCompTracks[iComponent].begin(); pIt != fCompTracks[iComponent].end(); ++pIt)
            {
                //std::cerr << "1" << std::endl;
                newPoint(0) = pIt->GetStartTimeInRunC() * scale(0); // start time
                //std::cerr << "2" << std::endl;
                newPoint(1) = pIt->GetStartFrequency() * scale(1);  // start freq
                //std::cerr << "3" << std::endl;
                newPoint(2) = pIt->GetEndTimeInRunC() * scale(0);   // end time
                //std::cerr << "4" << std::endl;
                newPoint(3) = pIt->GetEndFrequency() * scale(1);    // end freq
                //std::cerr << "5" << std::endl;

#ifndef NDEBUG
                std::stringstream ptStr;
                ptStr << "Point -- before: (" << pIt->GetStartTimeInRunC() << ", " << pIt->GetStartFrequency() << ", " << pIt->GetEndTimeInRunC() << ", " << pIt->GetEndFrequency() << ")";
#endif
                KTDEBUG(tclog, ptStr.str() << " -- after: " << newPoint);
                normPoints[iPoint++] = newPoint;
            }

            DistanceMatrix distMat;
            distMat.ComputeDistances< TrackDistance< Point > >(normPoints);

            // do the clustering!
            KTINFO(tclog, "Starting DBSCAN");
            KTDBScan< DistanceMatrix >::DBSResults results;
            if (! dbScan.DoClustering(distMat, results))
            {
                KTERROR(tclog, "An error occurred while clustering");
                return false;
            }
            KTDEBUG(tclog, "DBSCAN finished");

            // loop over the clusters found, and create data objects for them
            KTDEBUG(tclog, "Found " << results.fClusters.size() << " clusters; creating candidate events");
            for (vector< KTDBScan< DistanceMatrix >::Cluster >::const_iterator clustIt = results.fClusters.begin(); clustIt != results.fClusters.end(); ++clustIt)
            {
                if (clustIt->empty())
                {
                    KTWARN(tclog, "Empty cluster");
                    continue;
                }

                KTDEBUG(tclog, "Creating event " << fDataCount << "; includes " << clustIt->size() << " tracks");

                ++fDataCount;

                KTDataPtr data(new KTData());

                KTMultiTrackEventData& eventData = data->Of< KTMultiTrackEventData >();
                eventData.SetComponent(iComponent);
                eventData.SetEventID(fDataCount);

                for (KTDBScan< DistanceMatrix >::Cluster::const_iterator pointIdIt = clustIt->begin(); pointIdIt != clustIt->end(); ++pointIdIt)
                {
                    eventData.AddTrack(fCompTracks[iComponent][*pointIdIt]);
                }

                eventData.ProcessTracks();

                fCandidates.insert(data);
                fEventSignal(data);
            } // loop over clusters

            fCompTracks[iComponent].clear();

        } // loop over components

        KTDEBUG(tclog, "Clustering complete");
        fClusterDoneSignal();

        return true;
    }

    void KTDBScanEventClustering::SetNComponents(unsigned nComps)
    {
        fCompTracks.resize(nComps, vector< KTProcessedTrackData >());
        return;
    }

} /* namespace Katydid */
