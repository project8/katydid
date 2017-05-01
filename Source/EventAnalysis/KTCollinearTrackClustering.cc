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
            fGroupingStatuses.clear();

            for( TrackSetCIt trackIt = compIt->begin(); trackIt != compIt->end(); ++trackIt )
            {
                fSlopes.push_back( trackIt->GetSlope() );
                fIntercepts.push_back( trackIt->GetIntercept() );
                fGroupingStatuses.push_back( fUNGROUPED );
            }

            std::vector< int > newCluster;
            while( GetNUngrouped() > 0 )
            {
                newCluster = FindCluster();

                Nymph::KTDataPtr data( new Nymph::KTData() );
                KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
                newTrack.SetComponent( component );

                KTProcessedTrackData* oldTrack;

                TrackSetCIt trackIt = compIt->begin();

                for( int i = 0; i < newCluster.size(); ++i )
                {
                    std::advance( trackIt, newCluster[i] );
                    oldTrack = new KTProcessedTrackData( *trackIt );

                    if( i == 0 || oldTrack->GetStartTimeInRunC() < newTrack.GetStartTimeInRunC() )
                    {
                        newTrack.SetStartTimeInRunC( oldTrack->GetStartTimeInRunC() );
                    }
                    if( i == 0 || oldTrack->GetEndTimeInRunC() > newTrack.GetEndTimeInRunC() )
                    {
                        newTrack.SetEndTimeInRunC( oldTrack->GetEndTimeInRunC() );
                    }
                    if( i == 0 || oldTrack->GetStartFrequency() < newTrack.GetStartFrequency() )
                    {
                        newTrack.SetStartFrequency( oldTrack->GetStartFrequency() );
                    }
                    if( i == 0 || oldTrack->GetEndFrequency() > newTrack.GetEndFrequency() )
                    {
                        newTrack.SetEndFrequency( oldTrack->GetEndFrequency() );
                    }

                    fGroupingStatuses[newCluster[i]] = fGROUPED;
                }

                ProcessNewTrack( newTrack );
                fTrackSignal( data );
            }


            ++component;
        } // for loop over components

        fDoneSignal();

        return true;
    }

    std::vector< int > KTCollinearTrackClustering::FindCluster()
    {
        double avgQ = 0., avgQ2 = 0., varQ;
        double avgF = 0., avgF2 = 0., varF;
        double totalVariance;

        double delta, bestDelta = 0.;
        int worstTrack = -1.;

        double nUngrouped = (double)GetNUngrouped();
        std::vector< int > cluster;

        int nTracks = fSlopes.size();

        for( int i = 0; i < nTracks; ++i )
        {
            if( fGroupingStatuses[i] != fUNGROUPED )
            {
                continue;
            }

            avgQ += fSlopes[i];
            avgQ2 += std::pow( fSlopes[i], 2 );

            avgF += fIntercepts[i];
            avgF2 += std::pow( fIntercepts[i], 2 );

            cluster.push_back( i );
        }

        avgQ /= nUngrouped;
        avgQ2 /= nUngrouped;

        avgF /= nUngrouped;
        avgF2 /= nUngrouped;

        varQ = avgQ2 - std::pow( avgQ, 2 );
        varF = avgF2 - std::pow( avgF, 2 );

        totalVariance = varQ / std::pow( fSlopeRadius, 2 ) + varF / std::pow( fFrequencyRadius, 2 );

        if( totalVariance > std::sqrt( nUngrouped ) )
        {
            for( int i = 0; i < nTracks; ++i )
            {
                if( fGroupingStatuses[i] != fUNGROUPED )
                {
                    continue;
                }
                
                delta = std::pow( fSlopeRadius, -2 ) * ( (2. - nUngrouped) * std::pow( fSlopes[i], 2 ) + 2. * nUngrouped * avgQ * fSlopes[i] ) + std::pow( fFrequencyRadius, -2 ) * ( (2. - nUngrouped) * std::pow( fIntercepts[i], 2 ) + 2. * nUngrouped * avgF * fIntercepts[i] ); 
                if( i == 0 || delta > bestDelta )
                {
                    bestDelta = delta;
                    worstTrack = i;
                }
            }

            fGroupingStatuses[worstTrack] = fREMOVED;
            return FindCluster();
        }
        else
        {
            for( int i = 0; i < nTracks; ++i )
            {
                if( fGroupingStatuses[i] == fREMOVED )
                {
                    fGroupingStatuses[i] = fUNGROUPED;
                }
            }

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
