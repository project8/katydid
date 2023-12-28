/*
 * KTDBSCANEventClustering.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: N.S. Oblath
 */

#include "KTDBSCANEventClustering.hh"
#include "logger.hh"
#include "KTMath.hh"
#include "KTMultiTrackEventData.hh"
#include "KTProcessedTrackData.hh"

#ifndef NDEBUG
#include <sstream>
#endif

using std::set;
using std::vector;

namespace Katydid
{
    LOGGER(tclog, "katydid.fft");

    KT_REGISTER_PROCESSOR(KTDBSCANEventClustering, "dbscan-event-clustering");

    // dimensions: (t_start, f_start, t_end, f_end)
    const unsigned KTDBSCANEventClustering::fNDimensions = 4;
    // points in a track: (start, end)
    const unsigned KTDBSCANEventClustering::fNPointsPerTrack = 2;

    KTDBSCANEventClustering::KTDBSCANEventClustering(const std::string& name) :
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
            fTakeTrackSlot("track", this, &KTDBSCANEventClustering::TakeTrack)
    //        fDoClusterSlot("do-cluster-trigger", this, &KTDBSCANEventClustering::Run)
    {
        RegisterSlot("do-clustering", this, &KTDBSCANEventClustering::DoClusteringSlot);
        fRadii(0) = 1. / sqrt(fNDimensions);
        fRadii(1) = 1. / sqrt(fNDimensions);
    }

    KTDBSCANEventClustering::~KTDBSCANEventClustering()
    {
    }

    bool KTDBSCANEventClustering::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMinPoints(node->get_value("min-points", GetMinPoints()));

        if (node->has("radii"))
        {
            const scarab::param_array* radii = node->array_at("radii");
            if (radii->size() != fNDimensions / fNPointsPerTrack)
            {
                LERROR(tclog, "Radii array does not have the right number of dimensions: <" << radii->size() << "> instead of <" << fNDimensions/fNPointsPerTrack << ">");
                return false;
            }
            fRadii(0) = radii->get_value< double >(0);
            fRadii(1) = radii->get_value< double >(1);
        }

        return true;
    }

    bool KTDBSCANEventClustering::TakeTrack(KTProcessedTrackData& track)
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

        LDEBUG(tclog, "Taking track: (" << track.GetStartTimeInRunC() << ", " << track.GetStartFrequency() << ", " << track.GetEndTimeInRunC() << ", " << track.GetEndFrequency());

        return true;
    }

    /*
    bool KTDBSCANEventClustering::TakePoint(double time, double frequency *//*, double amplitude*//*, unsigned component)
    {
        if (component >= fCompPoints.size())
        {
            SetNComponents(component + 1);
        }

        KTDBSCAN::Point newPoint(fNDimensions);
        newPoint(0) = time;
        newPoint(1) = frequency;
        fCompPoints[component].push_back(newPoint);

        LDEBUG(tclog, "Point " << fCompPoints[component].size()-1 << " is now " << fCompPoints[component].back());

        return true;
    }
    */

    void KTDBSCANEventClustering::DoClusteringSlot()
    {
        if (! Run())
        {
            LERROR(tclog, "An error occurred while running the event clustering");
        }
        return;
    }

    bool KTDBSCANEventClustering::Run()
    {
        return DoClustering();
    }

    bool KTDBSCANEventClustering::DoClustering()
    {
        KTPROG(tclog, "Starting DBSCAN event clustering");

        KTDBSCAN< DistanceMatrix > dbScan;

        dbScan.SetRadius(1.);
        dbScan.SetMinPoints(fMinPoints);
        LINFO(tclog, "DBSCAN configured");

        for (unsigned iComponent = 0; iComponent < fCompTracks.size(); ++iComponent)
        {
            LDEBUG(tclog, "Clustering component " << iComponent);

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
            LDEBUG(tclog, "Scale: " << scale);
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
                LDEBUG(tclog, ptStr.str() << " -- after: " << newPoint);
                normPoints[iPoint++] = newPoint;
            }

            DistanceMatrix distMat;
            distMat.ComputeDistances< TrackDistance< Point > >(normPoints);

            // do the clustering!
            LINFO(tclog, "Starting DBSCAN");
            KTDBSCAN< DistanceMatrix >::DBSResults results;
            if (! dbScan.DoClustering(distMat, results))
            {
                LERROR(tclog, "An error occurred while clustering");
                return false;
            }
            LDEBUG(tclog, "DBSCAN finished");

            // loop over the clusters found, and create data objects for them
            LDEBUG(tclog, "Found " << results.fClusters.size() << " clusters; creating candidate events");
            for (vector< KTDBSCAN< DistanceMatrix >::Cluster >::const_iterator clustIt = results.fClusters.begin(); clustIt != results.fClusters.end(); ++clustIt)
            {
                if (clustIt->empty())
                {
                    LWARN(tclog, "Empty cluster");
                    continue;
                }

                LDEBUG(tclog, "Creating event " << fDataCount << "; includes " << clustIt->size() << " tracks");

                ++fDataCount;

                Nymph::KTDataPtr data(new Nymph::KTData());

                KTMultiTrackEventData& eventData = data->Of< KTMultiTrackEventData >();
                eventData.SetComponent(iComponent);
                eventData.SetAcquisitionID(fCompTracks[0][0].GetAcquisitionID());
                eventData.SetEventID(fDataCount);

                for (KTDBSCAN< DistanceMatrix >::Cluster::const_iterator pointIdIt = clustIt->begin(); pointIdIt != clustIt->end(); ++pointIdIt)
                {
                    eventData.AddTrack(fCompTracks[iComponent][*pointIdIt]);
                }

                eventData.ProcessTracks();

                fCandidates.insert(data);
                fEventSignal(data);
            } // loop over clusters

            fCompTracks[iComponent].clear();

        } // loop over components

        LDEBUG(tclog, "Clustering complete");
        fClusterDoneSignal();

        return true;
    }

    void KTDBSCANEventClustering::SetNComponents(unsigned nComps)
    {
        fCompTracks.resize(nComps, vector< KTProcessedTrackData >());
        return;
    }

} /* namespace Katydid */
