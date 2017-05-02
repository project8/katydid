/*
 * KTCollinearTrackClustering.cc
 *
 *  Created on: May 1, 2017
 *      Author: E. Zayas
 */

#include "KTCollinearTrackClustering.hh"

#include "KTLogger.hh"

#include <list>
#include <vector>
#include <cmath>

#ifndef NDEBUG
#include <sstream>
#endif

using std::list;
using std::set;

namespace Katydid
{
    KTLOGGER(tclog, "KTCollinearTrackClustering");

    KT_REGISTER_PROCESSOR(KTCollinearTrackClustering, "collinear-track-clustering");

    KTCollinearTrackClustering::KTCollinearTrackClustering(const std::string& name) :
            KTPrimaryProcessor(name),
            fSlopeRadius(1.),
            fFrequencyRadius(1.),
            fCompTracks(1),
            fSlopes(1),
            fIntercepts(1),
            fTimeLengths(1),
            fTrackSignal("track", this),
            fDoneSignal("tracks-done", this),
            fTakeTrackSlot("track", this, &KTCollinearTrackClustering::TakeTrack)
    {
        RegisterSlot("do-clustering", this, &KTCollinearTrackClustering::DoClusteringSlot);
    }

    KTCollinearTrackClustering::~KTCollinearTrackClustering()
    {
    }

    bool KTCollinearTrackClustering::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSlopeRadius(node->get_value("slope-radius", GetSlopeRadius()));
        SetFrequencyRadius(node->get_value("frequency-radius", GetFrequencyRadius()));

        return true;
    }

    bool KTCollinearTrackClustering::TakeTrack(KTProcessedTrackData& track)
    {
        // ignore the track if it's been cut
        if (track.GetIsCut()) return true;

        // verify that we have the right number of components
        if (track.GetComponent() >= fCompTracks.size())
        {
            SetNComponents(track.GetComponent() + 1);
        }

        KTDEBUG(tclog, "Taking track: (" << track.GetStartTimeInRunC() << ", " << track.GetStartFrequency() << ", " << track.GetEndTimeInRunC() << ", " << track.GetEndFrequency() << ")");

        // copy the full track data
        fCompTracks[track.GetComponent()].insert(track);

        return true;
    }

    void KTCollinearTrackClustering::DoClusteringSlot()
    {
        if (! Run())
        {
            KTERROR(tclog, "An error occurred while running the collinear track clustering");
        }
        return;
    }

    bool KTCollinearTrackClustering::Run()
    {
        return DoClustering();
    }

    bool KTCollinearTrackClustering::DoClustering()
    {
        if (! FindCollinearTracks())
        {
            KTERROR(tclog, "An error occurred while identifying collinear tracks");
            return false;
        }

        KTDEBUG(tclog, "Track building complete");
        fDoneSignal();

        return true;
    }

    bool KTCollinearTrackClustering::FindCollinearTracks()
    {
        KTINFO(tclog, "Finding collinear tracks");

        // loop over components
        unsigned component = 0;
        for (std::vector< TrackSet >::const_iterator compIt = fCompTracks.begin(); compIt != fCompTracks.end(); ++compIt)
        {
            KTDEBUG(tclog, "Doing component: " << compIt - fCompTracks.begin() + 1 << "/" << fCompTracks.size());
            
            fSlopes.clear();
            fIntercepts.clear();
            fTimeLengths.clear();
            fGroupingStatuses.clear();

            KTDEBUG(tclog, "Filling vectors with slope and intercept");
            for( TrackSetCIt trackIt = compIt->begin(); trackIt != compIt->end(); ++trackIt )
            {
                fSlopes.push_back( trackIt->GetSlope() );
                fIntercepts.push_back( trackIt->GetIntercept() );
                fTimeLengths.push_back( trackIt->GetTimeLength() );
                fGroupingStatuses.push_back( fUNGROUPED );
            }

            std::vector< int > newCluster;
            while( GetNUngrouped() > 0 )
            {
                // Find a new cluster of tracks
                newCluster = FindCluster();

                KTINFO(tclog, "New cluster has size " << newCluster.size());

                KTDEBUG(tclog, "Setting up new KTProcessedTrackData");

                // Set up new data object
                Nymph::KTDataPtr data( new Nymph::KTData() );
                KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
                newTrack.SetComponent( component );

                KTDEBUG(tclog, "Initalized new track");

                TrackSetCIt trackIt = compIt->begin();

                // Loop through tracks in the cluster
                for( int i = 0; i < newCluster.size(); ++i )
                {
                    KTDEBUG(tclog, "Advancing to position " << newCluster[i] );
                    // Get this track from the iterator

                    if( i == 0 )
                    {
                        std::advance( trackIt, newCluster[i] );
                    }
                    else
                    {
                        std::advance( trackIt, newCluster[i] - newCluster[i-1] );
                    }

                    // Assign start and end time/frequency

                    if( i == 0 || trackIt->GetStartTimeInRunC() < newTrack.GetStartTimeInRunC() )
                    {
                        newTrack.SetStartTimeInRunC( trackIt->GetStartTimeInRunC() );
                    }
                    if( i == 0 || trackIt->GetEndTimeInRunC() > newTrack.GetEndTimeInRunC() )
                    {
                        newTrack.SetEndTimeInRunC( trackIt->GetEndTimeInRunC() );
                    }
                    if( i == 0 || trackIt->GetStartFrequency() < newTrack.GetStartFrequency() )
                    {
                        newTrack.SetStartFrequency( trackIt->GetStartFrequency() );
                    }
                    if( i == 0 || trackIt->GetEndFrequency() > newTrack.GetEndFrequency() )
                    {
                        newTrack.SetEndFrequency( trackIt->GetEndFrequency() );
                    }

                    KTDEBUG(tclog, "Assigned time and frequency");

                    // Set tracks in this cluster to GROUPED
                    // They will no longer be considered for future clusters

                    fGroupingStatuses[newCluster[i]] = fGROUPED;

                    KTDEBUG(tclog, "Assigned GROUPED status");
                }

                // Process & emit new track

                KTINFO(tclog, "Set clustered tracks to GROUPED. Now processing new track");
                ProcessNewTrack( newTrack );

                KTDEBUG(tclog, "Emitting track signal");
                fTrackSignal( data );

                // If there are remaining UNGROUPED tracks, the while loop will repeat
                // If all tracks are GROUPED, we will exit this loop
            }


            ++component;
        } // for loop over components

        KTINFO(tclog, "Collinear track clustering finished. Emitting done signal");
        fDoneSignal();

        return true;
    }

    std::vector< int > KTCollinearTrackClustering::FindCluster()
    {
        // Initialize variables for variance and mean
        double avgQ = 0., avgQ2 = 0., varQ = 0.;
        double avgF = 0., avgF2 = 0., varF = 0.;
        double totalVariance;

        // Stuff for finding the worst track
        double delta, bestDelta = 0.;
        int worstTrack = -1.;

        // Having nUngrouped will be handy without calling the thing all the time
        double nUngrouped = (double)GetNUngrouped();

        // Vector of positions to describe the cluster
        std::vector< int > cluster;

        int nTracks = fSlopes.size();

        for( int i = 0; i < nTracks; ++i )
        {
            // Consider only ungrouped tracks
            if( fGroupingStatuses[i] != fUNGROUPED )
            {
                continue;
            }

            // Add to <q> and <q^2>, same for frequency

            avgQ += fSlopes[i];

            avgF += fIntercepts[i];
            avgF2 += std::pow( fIntercepts[i], 2 );

            // Add this index to the cluster
            cluster.push_back( i );
        }


        // Normalize expectation values and compure variances

        avgQ /= nUngrouped;

        avgF /= nUngrouped;
        avgF2 /= nUngrouped;

        for( int i = 0; i < nTracks; ++i )
        {
            // Consider only ungrouped tracks
            if( fGroupingStatuses[i] != fUNGROUPED )
            {
                continue;
            }

            varQ += std::pow( (fSlopes[i] - avgQ) * fTimeLengths[i], 2 );
        }

        varQ /= nUngrouped;
        varF = avgF2 - std::pow( avgF, 2 );

        KTDEBUG(tclog, "Slope variance = " << varQ);
        KTDEBUG(tclog, "Frequency variance = " << varF);

        // Weighted total variance
        totalVariance = varQ / std::pow( fSlopeRadius, 2 ) + varF / std::pow( fFrequencyRadius, 2 );

        KTINFO(tclog, "Computed total weighted variance = " << totalVariance);

        // Variance ~ Sqrt(N), so the threshold should as well
        if( totalVariance > nUngrouped )
        {
            // Variance is too big
            KTDEBUG(tclog, "Tracks not yet clustered. Finding worst track");

            // Find the track which is contributing the most to the variance
            for( int i = 0; i < nTracks; ++i )
            {
                // Again only ungrouped tracks
                if( fGroupingStatuses[i] != fUNGROUPED )
                {
                    continue;
                }
                
                // Maximum delta = worst track
                delta = std::pow( (fSlopes[i] - avgQ) * fTimeLengths[i] / fSlopeRadius, 2 ) + std::pow( (fIntercepts[i] - avgF) / fFrequencyRadius, 2 ); 
                
                // Compare to the current maximum delta
                if( delta > bestDelta )
                {
                    bestDelta = delta;
                    worstTrack = i;
                }
            }

            KTDEBUG(tclog, "Worst track index = " << worstTrack);
            KTDEBUG(tclog, "Best delta = " << bestDelta);

            // Set this one's status to REMOVED.
            fGroupingStatuses[worstTrack] = fREMOVED;

            // Now we will recursively repeat this calculation. The track which has just been set to REMOVED will now be skipped
            // Note that if there is only 1 track which is ungrouped, the variances will be zero and necessarily below the threshold

            KTDEBUG(tclog, "Removed worst track. Recursively continuing the cluster search");
            return FindCluster();
        }
        else
        {
            // Variance is small enough to consider these tracks collinear
            KTDEBUG(tclog, "Found a cluster! Reverting all removed tracks to UNGROUPED");

            // Reset all REMOVED tracks to UNGROUPED. They will be considered for future clusters
            for( int i = 0; i < nTracks; ++i )
            {
                if( fGroupingStatuses[i] == fREMOVED )
                {
                    fGroupingStatuses[i] = fUNGROUPED;
                }
            }

            // Return this cluster
            return cluster;
        }
    }

    void KTCollinearTrackClustering::ProcessNewTrack( KTProcessedTrackData& myNewTrack )
    {
        myNewTrack.SetTimeLength( myNewTrack.GetEndTimeInRunC() - myNewTrack.GetStartTimeInRunC() );
        myNewTrack.SetFrequencyWidth( myNewTrack.GetEndFrequency() - myNewTrack.GetStartFrequency() );
        myNewTrack.SetSlope( myNewTrack.GetFrequencyWidth() / myNewTrack.GetTimeLength() );
        myNewTrack.SetIntercept( myNewTrack.GetStartFrequency() - myNewTrack.GetSlope() * myNewTrack.GetStartTimeInRunC() );
    }

    int KTCollinearTrackClustering::GetNUngrouped()
    {
        int nUngrouped = 0;
        for( int i = 0; i < fSlopes.size(); ++i )
        {
            if( fGroupingStatuses[i] == fUNGROUPED )
            {
                nUngrouped++;
            }
        }

        KTDEBUG(tclog, "Tracks ungrouped: " << nUngrouped);

        return nUngrouped;
    }

    void KTCollinearTrackClustering::SetNComponents(unsigned nComps)
    {
        int fCTSize = fCompTracks.size();
        TrackSet blankSet;
        for( int i = fCTSize; i <= nComps; ++i )
            fCompTracks.push_back( blankSet );
    }

} /* namespace Katydid */
