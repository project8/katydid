/*
 * KTSpectrogramCollector.cc
 *
 *  Created on: Oct 9, 2015
 *      Author: ezayas
 */

#include "KTSpectrogramCollector.hh"

#include "logger.hh"

#include "KTSpectrumCollectionData.hh"
#include "KTProcessedTrackData.hh"
#include "KTMultiTrackEventData.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTData.hh"
#include "KTMath.hh"
#include "KTMultiPSData.hh"

#include <set>
#include <algorithm>

namespace Katydid
{
    LOGGER(evlog, "KTSpectrogramCollector");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTSpectrogramCollector, "spectrogram-collector");

    KTSpectrogramCollector::KTSpectrogramCollector(const std::string& name) :
            KTProcessor(name),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fLeadTime(0.),
            fTrailTime(0.),
            fLeadFreq(0.),
            fTrailFreq(0.),
            fUseTrackFreqs(false),
            fFullEvent(false),
            fPrevSliceTimeInRun(0.),
            fPrevSliceTimeInAcq(0.),
            fNSpectrograms(0),
            fWaterfallSignal("ps-coll", this),
            fTrackSlot("track", this, &KTSpectrogramCollector::ReceiveTrack),
            fMPTrackSlot("mp-track", this, &KTSpectrogramCollector::ReceiveMPTrack),
            fMPEventSlot("mp-event", this, &KTSpectrogramCollector::ReceiveMPEvent)
    {
        RegisterSlot( "ps", this, &KTSpectrogramCollector::SlotFunctionPSData );
    }

    KTSpectrogramCollector::~KTSpectrogramCollector()
    {
    }

    // Emit Signal
    void KTSpectrogramCollector::FinishSC( Nymph::KTDataPtr data, unsigned comp )
    {
        fWaterfallSignal( data );
    }

    bool KTSpectrogramCollector::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (node->has("min-frequency"))
        {
            SetMinFrequency(node->get_value< double >("min-frequency"));
        }
        if (node->has("max-frequency"))
        {
            SetMaxFrequency(node->get_value< double >("max-frequency"));
        }

        SetMinBin(node->get_value< unsigned >("min-bin", fMinBin));
        SetMaxBin(node->get_value< unsigned >("max-bin", fMaxBin));
        SetLeadTime(node->get_value< double >("lead-time", fLeadTime));
        SetTrailTime(node->get_value< double >("trail-time", fTrailTime));
        SetLeadFreq(node->get_value< double >("lead-freq", fLeadFreq));
        SetTrailFreq(node->get_value< double >("trail-freq", fTrailFreq));
        SetUseTrackFreqs(node->get_value< bool >("use-track-freqs", fUseTrackFreqs));
        SetFullEvent(node->get_value< bool >("full-event", fFullEvent));

        return true;
    }

    bool KTSpectrogramCollector::AddTrack( KTProcessedTrackData& trackData, unsigned component )
    {
        // Create new Nymph::KTDataPtr and its contents
        Nymph::KTDataPtr ptr( new Nymph::KTData() );
        KTProcessedTrackData* newTrack = &ptr->Of< KTProcessedTrackData >();
        KTPSCollectionData* newWaterfall = &ptr->Of< KTPSCollectionData >();
        newWaterfall->SetNComponents(1);

        // Configure the track to retain all of the old information
        *newTrack = trackData;

        // Configure PSCollectionData timestamps
        newWaterfall->SetStartTime( trackData.GetStartTimeInRunC() - fLeadTime );
        newWaterfall->SetEndTime( trackData.GetEndTimeInRunC() + fTrailTime );

        LINFO(evlog, "Set start time: " << newWaterfall->GetStartTime());
        LINFO(evlog, "Set end time: " << newWaterfall->GetEndTime());

        // Configure PSCollectionData frequency bounds
        if( GetUseTrackFreqs() )
        {
            newWaterfall->SetMinFreq( std::max( trackData.GetStartFrequency() - GetLeadFreq(), GetMinFrequency() ) );
            newWaterfall->SetMaxFreq( std::min( trackData.GetEndFrequency() + GetTrailFreq(), GetMaxFrequency() ) );
        }
        else
        {
            newWaterfall->SetMinFreq( GetMinFrequency() );
            newWaterfall->SetMaxFreq( GetMaxFrequency() );
        }

        newWaterfall->SetFilling( false );

        // It is possible that from the above logic, the minimum frequency is greater than the maximum
        // When this is the case, the KTPSCollectionData will be empty and present a high risk for crashes
        // Since it will never be filled, I will simply avoid adding it in the first place

        if( newWaterfall->GetMinFreq() > newWaterfall->GetMaxFreq() )
        {
            LWARN(evlog, "Spectrogram frequency bounds are not compatible! Will not collect this track");
            return false;
        }

        // Add to fWaterfallSets
        fWaterfallSets[component].insert( std::make_pair( ptr, newWaterfall ) );

        LINFO(evlog, "Added track to component " << component << ". Now listening to a total of " << fWaterfallSets[component].size() << " tracks");
        LINFO(evlog, "Track length: " << trackData.GetEndTimeInRunC() - trackData.GetStartTimeInRunC());
        LINFO(evlog, "Track slope: " << trackData.GetSlope());

        return true;
    }

    bool KTSpectrogramCollector::AddMPTrack( KTMultiPeakTrackData& mpTrackData, unsigned component )
    {
        LDEBUG(evlog, "Adding MP Track");
        // Create new Nymph::KTDataPtr and its contents
        Nymph::KTDataPtr ptr( new Nymph::KTData() );
        KTProcessedTrackData* newTrack = &ptr->Of< KTProcessedTrackData >();
        KTPSCollectionData* newWaterfall = &ptr->Of< KTPSCollectionData >();

        // start/end time and frequency for "track" equivalent object to collect
        double minStartFrequency = -1.;
        double maxStartFrequency = -1.;
        double minEndFrequency = -1.;
        double maxEndFrequency = -1.;
        double overallStartTime = -1.;
        double overallEndTime = -1.;

        double averageSlope = 0.;
        int mpt = 0;

        TrackSetCItSet allTracks = mpTrackData.GetMPTrack().fTrackRefs;
        for( TrackSetCItSet::iterator it = allTracks.begin(); it != allTracks.end(); ++it)
        {
            // Get track time and frequency info
            KTProcessedTrackData& aTrack = (*it)->fProcTrack;

            // Assign overall start/end time and frequency
            if( overallStartTime < 0 || aTrack.GetStartTimeInRunC() < overallStartTime )
            {
                overallStartTime = aTrack.GetStartTimeInRunC();
            }
            if( overallEndTime < 0 || aTrack.GetStartTimeInRunC() + aTrack.GetTimeLength() > overallEndTime )
            {
                overallEndTime = aTrack.GetStartTimeInRunC() + aTrack.GetTimeLength();
            }

            if( minStartFrequency < 0 || aTrack.GetStartFrequency() < minStartFrequency )
            {
                minStartFrequency = aTrack.GetStartFrequency();
            }
            if( maxStartFrequency < 0 || aTrack.GetStartFrequency() > maxStartFrequency )
            {
                maxStartFrequency = aTrack.GetStartFrequency();
            }

            if( minEndFrequency < 0 || aTrack.GetEndFrequency() < minEndFrequency )
            {
                minEndFrequency = aTrack.GetEndFrequency();
            }
            if( maxEndFrequency < 0 || aTrack.GetEndFrequency() > maxEndFrequency )
            {
                maxEndFrequency = aTrack.GetEndFrequency();
            }

            averageSlope += aTrack.GetSlope();
            ++mpt;
        }
        averageSlope /= (double)mpt;

        // Compute middle frequency values
        double midStartFrequency = 0.5 * (minStartFrequency + maxStartFrequency);
        double midEndFrequency = 0.5 * (minEndFrequency + maxEndFrequency);

        // Fill out newTrack
        newTrack->SetStartFrequency( midStartFrequency );
        newTrack->SetEndFrequency( midEndFrequency );
        newTrack->SetIntercept( midStartFrequency - overallStartTime * (midEndFrequency - midStartFrequency)/(overallEndTime - overallStartTime) );
        newTrack->SetSlope( averageSlope );

        LINFO(evlog, "Finished searching track set. MPT = " << mpt);

        // Configure PSCollectionData timestamps
        newWaterfall->SetStartTime( overallStartTime - fLeadTime );
        newWaterfall->SetEndTime( overallEndTime + fTrailTime );

        // Configure PSCollectionData frequency bounds
        if( GetUseTrackFreqs() )
        {
            newWaterfall->SetMinFreq( std::max( minStartFrequency - GetLeadFreq(), GetMinFrequency() ) );
            newWaterfall->SetMaxFreq( std::min( maxEndFrequency + GetTrailFreq(), GetMaxFrequency() ) );
        }
        else
        {
            newWaterfall->SetMinFreq( GetMinFrequency() );
            newWaterfall->SetMaxFreq( GetMaxFrequency() );
        }

        newWaterfall->SetFilling( false );

        // It is possible that from the above logic, the minimum frequency is greater than the maximum
        // When this is the case, the KTPSCollectionData will be empty and present a high risk for crashes
        // Since it will never be filled, I will simply avoid adding it in the first place

        if( newWaterfall->GetMinFreq() > newWaterfall->GetMaxFreq() )
        {
            LWARN(evlog, "Spectrogram frequency bounds are not compatible! Will not collect this track");
            return false;
        }

        // Also make sure none of them are still -1
        if( overallStartTime < 0 || overallEndTime < 0 || minStartFrequency < 0 || minEndFrequency < 0 || maxStartFrequency < 0 || maxEndFrequency < 0 )
        {
            LWARN(evlog, "Could not establish overall time and frequency bounds from multi-peak event! Will not collect this track");
            return false;
        }

        // Add to fWaterfallSets
        fWaterfallSets[component].insert( std::make_pair( ptr, newWaterfall ) );

        LINFO(evlog, "Added track to component " << component << ". Now listening to a total of " << fWaterfallSets[component].size() << " tracks");
        LINFO(evlog, "Track length: " << overallEndTime - overallStartTime);

        return true;   
    }

    bool KTSpectrogramCollector::AddMPEvent( KTMultiTrackEventData& mpEventData, unsigned component )
    {
        LDEBUG(evlog, "Adding MP Event");
        // Create new Nymph::KTDataPtr and its contents
        Nymph::KTDataPtr ptr( new Nymph::KTData() );
        KTProcessedTrackData* newTrack = &ptr->Of< KTProcessedTrackData >();
        KTPSCollectionData* newWaterfall = &ptr->Of< KTPSCollectionData >();

        // start/end time and frequency for "track" equivalent object to collect
        double overallStartFrequency = -1.;
        double overallEndFrequency = -1.;
        double overallStartTime = -1.;
        double overallEndTime = -1.;

        // start/end time and frequency for the "track" equivalent to the average in a MPT group
        // we will copy the required quantities into newTrack
        double averageStartFrequency = 0.;
        double averageEndFrequency = 0.;
        double averageIntercept = 0.;
        double averageSlope = 0.;

        // multiplicity
        int mpt = 0;

        if( ! GetFullEvent() )
        {
            LINFO(evlog, "Searching for tracks with fEventSequenceID==0");

            // If we are collecting only the first group in the event, we must loop through and find those tracks
            TrackSet allTracks = mpEventData.GetTracksSet();
            for( TrackSet::const_iterator it = allTracks.begin(); it != allTracks.end(); ++it )
            {
                KTProcessedTrackData& aTrack = it->fProcTrack;
                
                // Skip tracks with fEventSequenceID != 0
                if( aTrack.GetEventSequenceID() != 0 )
                {
                    LDEBUG(evlog, "Event sequence ID = " << aTrack.GetEventSequenceID() << "; skipping this track");
                    continue;
                }

                // Assign overall start/end time and frequency
                if( overallStartTime < 0 || aTrack.GetStartTimeInRunC() < overallStartTime )
                {
                    overallStartTime = aTrack.GetStartTimeInRunC();
                }
                if( overallEndTime < 0 || aTrack.GetStartTimeInRunC() + aTrack.GetTimeLength() > overallEndTime )
                {
                    overallEndTime = aTrack.GetStartTimeInRunC() + aTrack.GetTimeLength();
                }
                if( overallStartFrequency < 0 || aTrack.GetStartFrequency() < overallStartFrequency )
                {
                    overallStartFrequency = aTrack.GetStartFrequency();
                }
                if( overallEndFrequency < 0 || aTrack.GetEndFrequency() > overallEndFrequency )
                {
                    overallEndFrequency = aTrack.GetEndFrequency();
                }
                
                // Add to the averages
                averageStartFrequency += aTrack.GetStartFrequency();
                averageEndFrequency += aTrack.GetEndFrequency();
                averageIntercept += aTrack.GetIntercept();
                averageSlope += aTrack.GetSlope();

                // Increment the track multiplicity
                ++mpt;
            }

            // Compute averages
            averageStartFrequency /= (double)mpt;
            averageEndFrequency /= (double)mpt;
            averageIntercept /= (double)mpt;
            averageSlope /= (double)mpt;

            // Fill out newTrack
            newTrack->SetStartFrequency( averageStartFrequency );
            newTrack->SetEndFrequency( averageEndFrequency );
            newTrack->SetIntercept( averageIntercept );
            newTrack->SetSlope( averageSlope );

            LINFO(evlog, "Finished searching track set. MPT = " << mpt);
        }
        else
        {
            // Otherwise we may simply assign them from the mpEventData object directly
            overallStartTime = mpEventData.GetStartTimeInRunC();
            overallEndTime = mpEventData.GetStartTimeInRunC() + mpEventData.GetTimeLength();
            overallStartFrequency = mpEventData.GetStartFrequency();
            overallEndFrequency = mpEventData.GetEndFrequency();
            mpt = mpEventData.GetNTracks();
        }

        // Configure PSCollectionData timestamps
        newWaterfall->SetStartTime( overallStartTime - fLeadTime );
        newWaterfall->SetEndTime( overallEndTime + fTrailTime );

        // Configure PSCollectionData frequency bounds
        if( GetUseTrackFreqs() )
        {
            newWaterfall->SetMinFreq( std::max( overallStartFrequency - GetLeadFreq(), GetMinFrequency() ) );
            newWaterfall->SetMaxFreq( std::min( overallEndFrequency + GetTrailFreq(), GetMaxFrequency() ) );
        }
        else
        {
            newWaterfall->SetMinFreq( GetMinFrequency() );
            newWaterfall->SetMaxFreq( GetMaxFrequency() );
        }

        newWaterfall->SetFilling( false );

        // It is possible that from the above logic, the minimum frequency is greater than the maximum
        // When this is the case, the KTPSCollectionData will be empty and present a high risk for crashes
        // Since it will never be filled, I will simply avoid adding it in the first place

        if( newWaterfall->GetMinFreq() > newWaterfall->GetMaxFreq() )
        {
            LWARN(evlog, "Spectrogram frequency bounds are not compatible! Will not collect this track");
            return false;
        }

        // Also make sure none of them are still -1
        if( overallStartTime < 0 || overallEndTime < 0 || overallStartFrequency < 0 || overallEndFrequency < 0 )
        {
            LWARN(evlog, "Could not establish overall time and frequency bounds from multi-peak event! Will not collect this track");
            return false;
        }

        // Add to fWaterfallSets
        newWaterfall->SetSpectrogramCounter(fNSpectrograms);
        fWaterfallSets[component].insert( std::make_pair( ptr, newWaterfall ) );
        fNSpectrograms +=1;

        LINFO(evlog, "Added track to component " << component << ". Now listening to a total of " << fWaterfallSets[component].size() << " tracks");
        LINFO(evlog, "Track length: " << overallEndTime - overallStartTime);

        return true;
    }

    bool KTSpectrogramCollector::ConsiderSpectrum( KTPowerSpectrum& ps, KTSliceHeader& slice, unsigned component, bool forceEmit )
    {
        LDEBUG(evlog, "Now cross-checking slice timestamp with known tracks");
        // Iterate through each track which has been added
        for( auto it = fWaterfallSets[component].begin(); it != fWaterfallSets[component].end(); ++it )
        {
            if( it->second->GetDeltaT() < 0. )
            {
                LDEBUG(evlog, "This is the first time considering this collection");
                // then this is the first time this collection has been encountered, 
                // and we need to adjust its start and end times according to the slice length and time-in-run
                double deltaLeadTime = fLeadTime - KTMath::Nint(fLeadTime / slice.GetSliceLength()) * slice.GetSliceLength();
                double deltaTrailTime = KTMath::Nint(fTrailTime / slice.GetSliceLength()) * slice.GetSliceLength() - fTrailTime;
                //LWARN(evlog, fLeadTime << "  " << fLeadTime / slice.GetSliceLength() << "  " << KTMath::Nint(fLeadTime / slice.GetSliceLength()) << "  " << KTMath::Nint(fLeadTime / slice.GetSliceLength()) * slice.GetSliceLength() << "  " << fLeadTime - KTMath::Nint(fLeadTime / slice.GetSliceLength()) * slice.GetSliceLength() << "  " << deltaLeadTime);
                LDEBUG(evlog, "Adjusting lead time by " << deltaLeadTime << " s");
                LDEBUG(evlog, "Adjusting trail time by " << deltaTrailTime << " s");

                it->second->SetStartTime( it->second->GetStartTime() + deltaLeadTime );
                it->second->SetEndTime( it->second->GetEndTime() + deltaTrailTime );

                it->second->SetDeltaT( slice.GetSliceLength() );
            }

            double timeInRunC = slice.GetTimeInRun() + 0.5 * slice.GetSliceLength();
            LDEBUG(evlog, "slice's time in run-c: " << timeInRunC << ";  compared to track [" << it->second->GetStartTime() << ", " << it->second->GetEndTime() << "]");
            // If the slice time coincides with the track time window, add the spectrum
            // The forceEmit flag overrides this; essentially guarantees the spectrum will be interpreted as outside the track window
            // Overlap is defined by the slice end-time being greater than the collection start-time and the slice start-time being less than the collection end-time
            if( ! forceEmit && timeInRunC >= it->second->GetStartTime() && timeInRunC <= it->second->GetEndTime() )
            {
                LINFO(evlog, "Adding spectrum. Time in run = " << slice.GetTimeInRun());
                it->second->AddSpectrum( timeInRunC, ps, component );
                it->second->SetFilling( true );
            }
            else
            {
                // If GetFilling() is true, we've reached the end of the track time window
                // forceEmit=true sends all tracks to this clause, and those still filling will be closed & signals emitted
                if( it->second->GetFilling() )
                {
                    it->second->SetFilling( false );

                    // Emit signal
                    LINFO(evlog, "Finished a track; emitting signal");

                    LDEBUG(evlog, "Old collection start time: " << it->second->GetStartTime());
                    LDEBUG(evlog, "Old collection end time: " << it->second->GetEndTime());

                    LDEBUG(evlog, "Slice acquisition time:" << slice.GetTimeInAcq());

                    // Convert start and end times from run to acquistion
                    it->second->SetStartTime( it->second->GetStartTime() - it->second->GetEndTime() + fPrevSliceTimeInAcq );
                    it->second->SetEndTime( fPrevSliceTimeInAcq );

                    LINFO(evlog, "New collection start time (in acquisition): " << it->second->GetStartTime());
                    LINFO(evlog, "New collection end time (in acquisition): " << it->second->GetEndTime());

                    FinishSC( it->first, component );
                }
                else
                {
                    it->second->SetFilling( false );
                }
            }
        }

        SetPrevSliceTimeInRun( slice.GetTimeInRun() );
        SetPrevSliceTimeInAcq( slice.GetTimeInAcq() );

        LDEBUG(evlog, "Set previous slice time in run to " << fPrevSliceTimeInRun);
        LDEBUG(evlog, "Set previous slice time in acq to " << fPrevSliceTimeInAcq);

        return true;
    }

    bool KTSpectrogramCollector::ReceiveTrack( KTProcessedTrackData& data )
    {
        unsigned iComponent = data.GetComponent();

        // Increase size of fWaterfallSets if necessary
        int fWSsize = fWaterfallSets.size();
        std::set< std::pair< Nymph::KTDataPtr, KTPSCollectionData* >, KTTrackCompare > blankSet;
        for( int i = fWSsize; i <= iComponent; i++ )
        {
            fWaterfallSets.push_back( blankSet );
        }

        // Add track
        if( !AddTrack( data, iComponent ) )
        {
            LERROR(evlog, "Spectrogram collector could not add track! (component " << iComponent << ")" );
        }

        return true;
    }

    bool KTSpectrogramCollector::ReceiveMPTrack( KTMultiPeakTrackData& data )
    {
        unsigned iComponent = data.GetComponent();

        // Increase size of fWaterfallSets if necessary
        int fWSsize = fWaterfallSets.size();
        std::set< std::pair< Nymph::KTDataPtr, KTPSCollectionData* >, KTTrackCompare > blankSet;
        for( int i = fWSsize; i <= iComponent; i++ )
        {
            fWaterfallSets.push_back( blankSet );
        }

        // Add track
        if( !AddMPTrack( data, iComponent ) )
        {
            LERROR(evlog, "Spectrogram collector could not add multi-peak event! (component " << iComponent << ")" );
        }

        return true;
    }

    bool KTSpectrogramCollector::ReceiveMPEvent( KTMultiTrackEventData& data )
    {
        unsigned iComponent = data.GetComponent();

        // Increase size of fWaterfallSets if necessary
        int fWSsize = fWaterfallSets.size();
        std::set< std::pair< Nymph::KTDataPtr, KTPSCollectionData* >, KTTrackCompare > blankSet;
        for( int i = fWSsize; i <= iComponent; i++ )
        {
            fWaterfallSets.push_back( blankSet );
        }

        // Add track
        if( !AddMPEvent( data, iComponent ) )
        {
            LERROR(evlog, "Spectrogram collector could not add multi-peak event! (component " << iComponent << ")" );
        }

        return true;
    }
    
    bool KTSpectrogramCollector::ReceiveSpectrum( KTPowerSpectrumData& data, KTSliceHeader& sliceData, bool forceEmit )
    {
        LDEBUG(evlog, "Receiving Spectrum");
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrum(0)->FindBin(fMinFrequency));
            LDEBUG(evlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrum(0)->FindBin(fMaxFrequency));
            LDEBUG(evlog, "Maximum bin set to " << fMaxBin);
        }

        if( fWaterfallSets.empty() )
        {
            LWARN(evlog, "I have no tracks to receive a spectrum! Did you remember to send me processed tracks first? Continuing anyway...");
            return true;
        }

        int nComponents = data.GetNComponents();

        if( nComponents > fWaterfallSets.size() )
        {
            LINFO(evlog, "Receiving spectrum with " << nComponents << " components but limiting to " << fWaterfallSets.size() << " from list of tracks");
            nComponents = fWaterfallSets.size();
        }

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            if (! ConsiderSpectrum(*data.GetSpectrum(iComponent), sliceData, iComponent, forceEmit))
            {
                LERROR(evlog, "Spectrogram collector could not receive spectrum! (component " << iComponent << ")");
                return false;
            }
        }
        LINFO(evlog, "Spectrum finished processing. Awaiting next spectrum");

        return true;
    }
} // namespace Katydid
