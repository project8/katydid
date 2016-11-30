/*
 * KTSpectrogramCollector.cc
 *
 *  Created on: Oct 9, 2015
 *      Author: ezayas
 */

#include "KTSpectrogramCollector.hh"

#include "KTLogger.hh"

#include "KTSliceHeader.hh"
#include "KTSpectrumCollectionData.hh"
#include "KTProcessedTrackData.hh"
#include "KTMultiTrackEventData.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTData.hh"

#include <set>
#include <algorithm>

namespace Katydid
{
    KTLOGGER(evlog, "KTSpectrogramCollector");

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
            fWaterfallSignal("ps-coll", this),
            fTrackSlot("track", this, &KTSpectrogramCollector::ReceiveTrack),
            fMPEventSlot("mp-event", this, &KTSpectrogramCollector::ReceiveMPEvent)
    {
        RegisterSlot( "ps", this, &KTSpectrogramCollector::SlotFunctionPSData );
    }

    KTSpectrogramCollector::~KTSpectrogramCollector()
    {
    }

    // Emit Signal
    void KTSpectrogramCollector::FinishSC( Nymph::KTDataPtr data )
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

        return true;
    }

    bool KTSpectrogramCollector::AddTrack( KTProcessedTrackData& trackData, unsigned component )
    {
        // Create new Nymph::KTDataPtr and its contents
        Nymph::KTDataPtr ptr( new Nymph::KTData() );
        KTProcessedTrackData* newTrack = &ptr->Of< KTProcessedTrackData >();
        KTPSCollectionData* newWaterfall = &ptr->Of< KTPSCollectionData >();

        // Configure the track to retain all of the old information
        *newTrack = trackData;

        // Configure PSCollectionData timestamps
        newWaterfall->SetStartTime( trackData.GetStartTimeInRunC() - fLeadTime );
        newWaterfall->SetEndTime( trackData.GetEndTimeInRunC() + fTrailTime );

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
            KTWARN(evlog, "Spectrogram frequency bounds are not compatible! Will not collect this track");
            return false;
        }

        // Add to fWaterfallSets
        fWaterfallSets[component].insert( std::make_pair( ptr, newWaterfall ) );

        KTINFO(evlog, "Added track to component " << component << ". Now listening to a total of " << fWaterfallSets[component].size() << " tracks");
        KTINFO(evlog, "Track length: " << trackData.GetEndTimeInRunC() - trackData.GetStartTimeInRunC());
        KTINFO(evlog, "Track slope: " << trackData.GetSlope());

        return true;
    }

    bool KTSpectrogramCollector::AddMPEvent( KTMultiTrackEventData& mpEventData, unsigned component )
    {
        // Create new Nymph::KTDataPtr and its contents
        Nymph::KTDataPtr ptr( new Nymph::KTData() );
        KTProcessedTrackData* newTrack = &ptr->Of< KTProcessedTrackData >();
        KTPSCollectionData* newWaterfall = &ptr->Of< KTPSCollectionData >();

        // start/end time and frequency for "track" equivalent object to collect
        double overallStartFrequency = -1.;
        double overallEndFrequency = -1.;
        double overallStartTime = -1.;
        double overallEndTime = -1.;
        int mpt = 0;

        if( ! GetFullEvent() )
        {
            // If we are collecting only the first group in the event, we must loop through and find those tracks
            for( TrackSet::iterator it = mpEventData.GetTracksSet().begin(); it != mpEventData.GetTracksSet().end(); ++it )
            {
                KTProcessedTrackData aTrack = KTProcessedTrackData(*it);

                // Skip tracks with fEventSequenceID != 0
                if( aTrack.GetEventSequenceID() != 0 )
                {
                    continue;
                }

                // Assign overall start/end time and frequency
                if( overallStartTime < 0 || aTrack.GetStartTimeInRunC() < overallStartTime )
                {
                    overallStartTime = aTrack.GetStartTimeInRunC();
                }
                if( overallEndTime < 0 || aTrack.GetEndTimeInRunC() > overallEndTime )
                {
                    overallEndTime = aTrack.GetEndTimeInRunC();
                }
                if( overallStartFrequency < 0 || aTrack.GetStartFrequency() < overallStartFrequency )
                {
                    overallStartFrequency = aTrack.GetStartFrequency();
                }
                if( overallEndFrequency < 0 || aTrack.GetEndFrequency() > overallEndFrequency )
                {
                    overallEndFrequency = aTrack.GetEndFrequency();
                }

                // Increment the track multiplicity
                ++mpt;
            }
        }
        else
        {
            // Otherwise we may simply assign them from the mpEventData object directly
            overallStartTime = mpEventData.GetStartTimeInRunC();
            overallEndTime = mpEventData.GetEndTimeInRunC();
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
            KTWARN(evlog, "Spectrogram frequency bounds are not compatible! Will not collect this track");
            return false;
        }

        // Also make sure none of them are still -1
        if( overallStartTime < 0 || overallEndTime < 0 || overallStartFrequency < 0 || overallEndFrequency || 0 )
        {
            KTWARN(evlog, "Could not establish overall time and frequency bounds from multi-peak event! Will not collect this track");
            return false;
        }

        // ***** THIS PART TEMPORARY *****
        // Collect only tracks with MPT = 2
        if( mpt != 2 )
        {
            KTWARN(evlog, "The multiplicity of this event is " << mpt << "; will not collect it");
            return true;
        }

        // Add to fWaterfallSets
        fWaterfallSets[component].insert( std::make_pair( ptr, newWaterfall ) );

        KTINFO(evlog, "Added track to component " << component << ". Now listening to a total of " << fWaterfallSets[component].size() << " tracks");
        KTINFO(evlog, "Track length: " << overallEndTime - overallStartTime);

        return true;   
    }

    bool KTSpectrogramCollector::ConsiderSpectrum( KTPowerSpectrum& ps, KTSliceHeader& slice, unsigned component, bool forceEmit )
    {
        // Iterate through each track which has been added
        for( std::set< std::pair< Nymph::KTDataPtr, KTPSCollectionData* >, KTTrackCompare >::const_iterator it = fWaterfallSets[component].begin(); it != fWaterfallSets[component].end(); ++it )
        {
            // If the slice time coincides with the track time window, add the spectrum
            // The forceEmit flag overrides this; essentially guarantees the spectrum will be interpreted as outside the track window
            if( !forceEmit && slice.GetTimeInRun() >= it->second->GetStartTime() && slice.GetTimeInRun() <= it->second->GetEndTime() )
            {
                it->second->AddSpectrum( slice.GetTimeInRun(), &ps );
                it->second->SetDeltaT( slice.GetSliceLength() );
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
                    KTINFO(evlog, "Finished a track; emitting signal");
                    FinishSC( it->first );
                }
                else
                    it->second->SetFilling( false );
            }
        }

        return true;
    }

    bool KTSpectrogramCollector::ReceiveTrack( KTProcessedTrackData& data )
    {
        unsigned iComponent = data.GetComponent();

        // Increase size of fWaterfallSets if necessary
        int fWSsize = fWaterfallSets.size();
        std::set< std::pair< Nymph::KTDataPtr, KTPSCollectionData* >, KTTrackCompare > blankSet;
        for( int i = fWSsize; i <= iComponent; i++ )
            fWaterfallSets.push_back( blankSet );

        // Add track
        if( !AddTrack( data, iComponent ) )
        {
            KTERROR(evlog, "Spectrogram collector could not add track! (component " << iComponent << ")" );
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
            fWaterfallSets.push_back( blankSet );

        // Add track
        if( !AddMPEvent( data, iComponent ) )
        {
            KTERROR(evlog, "Spectrogram collector could not add multi-peak event! (component " << iComponent << ")" );
        }

        return true;
    }
    
    bool KTSpectrogramCollector::ReceiveSpectrum( KTPowerSpectrumData& data, KTSliceHeader& sliceData, bool forceEmit )
    {
        KTDEBUG(evlog, "Receiving Spectrum");
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrum(0)->FindBin(fMinFrequency));
            KTDEBUG(evlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrum(0)->FindBin(fMaxFrequency));
            KTDEBUG(evlog, "Maximum bin set to " << fMaxBin);
        }

        if( fWaterfallSets.empty() )
        {
            KTWARN(evlog, "I have no tracks to receive a spectrum! Did you remember to send me processed tracks first? Continuing anyway...");
            return true;
        }

        int nComponents = data.GetNComponents();

        if( nComponents > fWaterfallSets.size() )
        {
            KTINFO(evlog, "Receiving spectrum with " << nComponents << " components but limiting to " << fWaterfallSets.size() << " from list of tracks");
            nComponents = fWaterfallSets.size();
        }

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            if (! ConsiderSpectrum(*data.GetSpectrum(iComponent), sliceData, iComponent, forceEmit))
            {
                KTERROR(evlog, "Spectrogram collector could not receive spectrum! (component " << iComponent << ")");
                return false;
            }
        }
        KTINFO(evlog, "Spectrum finished processing. Awaiting next spectrum");

        return true;
    }
} // namespace Katydid
