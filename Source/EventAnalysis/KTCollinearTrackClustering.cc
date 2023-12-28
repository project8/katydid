/*
 * KTCollinearTrackClustering.cc
 *
 *  Created on: May 1, 2017
 *      Author: E. Zayas
 */

#include "KTCollinearTrackClustering.hh"

#include "logger.hh"

#include <vector>

namespace Katydid
{
    LOGGER(tclog, "KTCollinearTrackClustering");

    KT_REGISTER_PROCESSOR(KTCollinearTrackClustering, "collinear-track-clustering");

    KTCollinearTrackClustering::KTCollinearTrackClustering(const std::string& name) :
            KTPrimaryProcessor(name),
            fSlopeRadius(1.),
            fFrequencyRadius(1.),
            fCompTracks(1),
            fSlopes(1),
            fIntercepts(1),
            fTimeLengths(1),
            fStartTimes(1),
            fTrackSignal("track", this),
            fDoneSignal("tracks-done", this),
            fTakeTrackSlot("track", this, &KTCollinearTrackClustering::TakeTrack)
    {
        fClusterFlag = false;
        fTerminateFlag = false;
        fWorstTrack = -1;

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

        LDEBUG(tclog, "Taking track: (" << track.GetStartTimeInRunC() << ", " << track.GetStartFrequency() << ", " << track.GetEndTimeInRunC() << ", " << track.GetEndFrequency() << ")");

        // copy the full track data

        Nymph::KTDataPtr data;
        KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
        newTrack = track;

        AllTrackData trackObject( data, newTrack );
        fCompTracks[track.GetComponent()].insert(trackObject);

        return true;
    }

    void KTCollinearTrackClustering::DoClusteringSlot()
    {
        if (! Run())
        {
            LERROR(tclog, "An error occurred while running the collinear track clustering");
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
            LERROR(tclog, "An error occurred while identifying collinear tracks");
            return false;
        }

        LDEBUG(tclog, "Track building complete");
        fDoneSignal();

        return true;
    }

    bool KTCollinearTrackClustering::FindCollinearTracks()
    {
        LINFO(tclog, "Finding collinear tracks");

        // loop over components
        unsigned component = 0;
        for (std::vector< TrackSet >::const_iterator compIt = fCompTracks.begin(); compIt != fCompTracks.end(); ++compIt)
        {
            LDEBUG(tclog, "Doing component: " << compIt - fCompTracks.begin() + 1 << "/" << fCompTracks.size());
            
            fSlopes.clear();
            fIntercepts.clear();
            fTimeLengths.clear();
            fStartTimes.clear();
            fGroupingStatuses.clear();

            LDEBUG(tclog, "Filling vectors with slope and intercept");
            for( TrackSetCIt trackIt = compIt->begin(); trackIt != compIt->end(); ++trackIt )
            {
                fSlopes.push_back( trackIt->fProcTrack.GetSlope() );
                fIntercepts.push_back( trackIt->fProcTrack.GetIntercept() );
                fTimeLengths.push_back( trackIt->fProcTrack.GetTimeLength() );
                fStartTimes.push_back( trackIt->fProcTrack.GetStartTimeInRunC() );
                fGroupingStatuses.push_back( fUNGROUPED );
            }

            while( GetNUngrouped() > 0 )
            {
                // Find a new cluster of tracks
                fClusterFlag = false;
                fTerminateFlag = false;

                while( ! fTerminateFlag )
                {
                    FindCluster();
                }

                LINFO(tclog, "New cluster has size " << fCluster.size());

                LDEBUG(tclog, "Setting up new KTProcessedTrackData");

                // Set up new data object
                Nymph::KTDataPtr data( new Nymph::KTData() );
                KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
                newTrack.SetComponent( component );

                LDEBUG(tclog, "Initalized new track");

                TrackSetCIt trackIt = compIt->begin();

                // Loop through tracks in the cluster
                for( int i = 0; i < fCluster.size(); ++i )
                {
                    LDEBUG(tclog, "Advancing to position " << fCluster[i] );
                    // Get this track from the iterator

                    if( i == 0 )
                    {
                        std::advance( trackIt, fCluster[i] );
                    }
                    else
                    {
                        std::advance( trackIt, fCluster[i] - fCluster[i-1] );
                    }

                    // Assign start and end time/frequency

                    if( i == 0 || trackIt->fProcTrack.GetStartTimeInRunC() < newTrack.GetStartTimeInRunC() )
                    {
                        newTrack.SetStartTimeInRunC( trackIt->fProcTrack.GetStartTimeInRunC() );
                    }
                    if( i == 0 || trackIt->fProcTrack.GetEndTimeInRunC() > newTrack.GetEndTimeInRunC() )
                    {
                        newTrack.SetEndTimeInRunC( trackIt->fProcTrack.GetEndTimeInRunC() );
                    }
                    if( i == 0 || trackIt->fProcTrack.GetStartFrequency() < newTrack.GetStartFrequency() )
                    {
                        newTrack.SetStartFrequency( trackIt->fProcTrack.GetStartFrequency() );
                    }
                    if( i == 0 || trackIt->fProcTrack.GetEndFrequency() > newTrack.GetEndFrequency() )
                    {
                        newTrack.SetEndFrequency( trackIt->fProcTrack.GetEndFrequency() );
                    }

                    LDEBUG(tclog, "Assigned time and frequency");

                    // Set tracks in this cluster to GROUPED
                    // They will no longer be considered for future clusters

                    fGroupingStatuses[fCluster[i]] = fGROUPED;

                    LDEBUG(tclog, "Assigned GROUPED status");
                }

                // Process & emit new track

                LINFO(tclog, "Set clustered tracks to GROUPED. Now processing new track");
                ProcessNewTrack( newTrack );

                LDEBUG(tclog, "Emitting track signal");
                fTrackSignal( data );

                // If there are remaining UNGROUPED tracks, the while loop will repeat
                // If all tracks are GROUPED, we will exit this loop
            }


            ++component;
        } // for loop over components

        LINFO(tclog, "Collinear track clustering finished. Emitting done signal");
        fDoneSignal();

        return true;
    }

    void KTCollinearTrackClustering::FindCluster()
    {
        LDEBUG(tclog, "Beginning cluster determination");
        
        // Initialize variables for variance and mean
        double avgQ = 0.;
        double varQ = 0.;
        double avgF = 0.;
        double varF = 0.;
        double totalVariance = 0.;

        // Stuff for finding the worst track
        double delta = 0.;
        double bestDelta = 0.;

        // Having nUngrouped will be handy without calling the thing all the time
        // Make it a double because I want to divide by it often
        double nUngrouped = (double)GetNUngrouped();

        fCluster.clear();

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

            // Add this index to the cluster
            fCluster.push_back( i );
        }


        // Normalize expectation values and compure variances

        avgQ /= (double)nUngrouped;
        avgF /= (double)nUngrouped;
        for( int i = 0; i < nTracks; ++i )
        {
            // Consider only ungrouped tracks
            if( fGroupingStatuses[i] != fUNGROUPED )
            {
                continue;
            }

            varQ += (fSlopes[i] - avgQ) * (fSlopes[i] - avgQ) * fTimeLengths[i] * fTimeLengths[i];
            varF += (fIntercepts[i] - avgF) * (fIntercepts[i] - avgF);
        }

        varQ /= (double)nUngrouped;
        varF /= (double)nUngrouped;

        LDEBUG(tclog, "Slope variance = " << varQ);
        LDEBUG(tclog, "Frequency variance = " << varF);

        // Weighted total variance
        totalVariance = varQ / (double)(fSlopeRadius * fSlopeRadius) + varF / (double)(fFrequencyRadius * fFrequencyRadius);

        LINFO(tclog, "Computed total weighted variance = " << totalVariance);

        // Variance ~ N, so the threshold should as well
        if( totalVariance <= nUngrouped )
        {
            // Variance is small enough to consider these tracks collinear
            LDEBUG(tclog, "Found a cluster! Trying to reinstate REMOVED tracks");

            fClusterFlag = true;
            bestDelta = 1.0e9;
            fWorstTrack = -1;

            for( int i = 0; i < nTracks; ++i )
            {
                // Again only ungrouped tracks
                if( fGroupingStatuses[i] != fREMOVED )
                {
                    continue;
                }
                
                // This time minimum delta = best track
                delta = (fSlopes[i] - avgQ) * (fSlopes[i] - avgQ) * fTimeLengths[i] * fTimeLengths[i] / (double)(fSlopeRadius * fSlopeRadius) + (fIntercepts[i] - avgF) * (fIntercepts[i] - avgF) / (double)(fFrequencyRadius * fFrequencyRadius); 

                // Compare to the current minimum delta
                if( delta < bestDelta )
                {
                    bestDelta = delta;
                    fWorstTrack = i; // best track
                }
            }

            if( fWorstTrack == -1 )
            {
                LDEBUG(tclog, "Didn't find any REMOVED tracks. Terminating cluster search");

                // Reset all REMOVED tracks to UNGROUPED. They will be considered for future clusters
                for( int i = 0; i < nTracks; ++i )
                {
                    if( fGroupingStatuses[i] == fREMOVED )
                    {
                        fGroupingStatuses[i] = fUNGROUPED;
                    }
                }

                fTerminateFlag = true;
            }
            else
            {
                LDEBUG(tclog, "Best track index = " << fWorstTrack);
                LDEBUG(tclog, "Best delta = " << bestDelta);

                // Reinstate this track
                fGroupingStatuses[fWorstTrack] = fUNGROUPED;

                LDEBUG(tclog, "Added best track, with intercept " << fIntercepts[fWorstTrack] << ". Recursively continuing the cluster search");
                fTerminateFlag = false;
            }
        }
        else
        {
            if( ! fClusterFlag )
            {
                // Variance is too big
                LDEBUG(tclog, "Tracks not yet clustered. Finding worst track");

                fWorstTrack = -1;

                // Find the track which is contributing the most to the variance
                for( int i = 0; i < nTracks; ++i )
                {
                    // Again only ungrouped tracks
                    if( fGroupingStatuses[i] != fUNGROUPED )
                    {
                        continue;
                    }
                    
                    // Maximum delta = worst track
                    delta = (fSlopes[i] - avgQ) * (fSlopes[i] - avgQ) * fTimeLengths[i] * fTimeLengths[i] / (double)(fSlopeRadius * fSlopeRadius) + (fIntercepts[i] - avgF) * (fIntercepts[i] - avgF) / (double)(fFrequencyRadius * fFrequencyRadius); 

                    // Compare to the current maximum delta
                    if( delta > bestDelta )
                    {
                        bestDelta = delta;
                        fWorstTrack = i;
                    }
                }

                LDEBUG(tclog, "Worst track index = " << fWorstTrack);
                LDEBUG(tclog, "Best delta = " << bestDelta);

                // Set this one's status to REMOVED.
                fGroupingStatuses[fWorstTrack] = fREMOVED;

                // Now we will recursively repeat this calculation. The track which has just been set to REMOVED will now be skipped
                // Note that if there is only 1 track which is ungrouped, the variances will be zero and necessarily below the threshold

                LDEBUG(tclog, "Removed worst track, with intercept " << fIntercepts[fWorstTrack] << ". Recursively continuing the cluster search");
                fTerminateFlag = false;
            }
            else
            {
                LDEBUG(tclog, "Variance exceeded limit; removing most recent track and terminating cluster search");

                fGroupingStatuses[fWorstTrack] = fREMOVED;
                
                for( std::vector< int >::iterator it = fCluster.begin(); it != fCluster.end(); ++it )
                {
                    if( *it == fWorstTrack )
                    {
                        LDEBUG(tclog, "Found worst track. Removing it now");
                        fCluster.erase( it );

                        --it;
                    }
                }

                // Reset all REMOVED tracks to UNGROUPED. They will be considered for future clusters
                for( int i = 0; i < nTracks; ++i )
                {
                    if( fGroupingStatuses[i] == fREMOVED )
                    {
                        fGroupingStatuses[i] = fUNGROUPED;
                    }
                }

                // Return this cluster
                fTerminateFlag = true;

            }   // end fClusterFlag conditional
        }       // end totalVariance conditional
    }           // end method

    void KTCollinearTrackClustering::ProcessNewTrack( KTProcessedTrackData& myNewTrack )
    {
        myNewTrack.SetTimeLength( myNewTrack.GetEndTimeInRunC() - myNewTrack.GetStartTimeInRunC() );
        myNewTrack.SetFrequencyWidth( myNewTrack.GetEndFrequency() - myNewTrack.GetStartFrequency() );
        myNewTrack.SetSlope( myNewTrack.GetFrequencyWidth() / myNewTrack.GetTimeLength() );
        myNewTrack.SetIntercept( myNewTrack.GetStartFrequency() - myNewTrack.GetSlope() * myNewTrack.GetStartTimeInRunC() );
    }

    int KTCollinearTrackClustering::GetNUngrouped()
    {
        LDEBUG(tclog, "Determining number of ungrouped tracks");

        int nUngrouped = 0;
        for( int i = 0; i < fSlopes.size(); ++i )
        {
            if( fGroupingStatuses[i] == fUNGROUPED )
            {
                nUngrouped++;
            }
        }

        LDEBUG(tclog, "Tracks ungrouped: " << nUngrouped);

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
