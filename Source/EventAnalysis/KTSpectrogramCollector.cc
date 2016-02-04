/*
 * KTSpectrogramCollector.cc
 *
 *  Created on: Oct 9, 2015
 *      Author: nsoblath
 */

#include "KTSpectrogramCollector.hh"

#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTParam.hh"
#include "KTSliceHeader.hh"
#include "KTSpectrumCollectionData.hh"
#include "KTProcessedTrackData.hh"
#include "KTPowerSpectrumData.hh"
#include "KTData.hh"


namespace Nymph
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
            fLeadTime(0.002),
            fTrailTime(0.002),
            fWaterfallSignal("waterfall", this),
            fTrackSlot("track", this, &KTSpectrogramCollector::ReceiveTrack, &fWaterfallSignal),
            fPSSlot("ps", this, &KTSpectrogramCollector::ReceiveSpectrum, &fWaterfallSignal)
    {
    }

    KTSpectrogramCollector::~KTSpectrogramCollector()
    {
    }

    bool KTSpectrogramCollector::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        if (node->Has("min-frequency"))
        {
            SetMinFrequency(node->GetValue< double >("min-frequency"));
        }
        if (node->Has("max-frequency"))
        {
            SetMaxFrequency(node->GetValue< double >("max-frequency"));
        }

        if (node->Has("min-bin"))
        {
            SetMinBin(node->GetValue< unsigned >("min-bin"));
        }
        if (node->Has("max-bin"))
        {
            SetMaxBin(node->GetValue< unsigned >("max-bin"));
        }

        if (node->Has("lead-time"))
        {
            SetLeadTime(node->GetValue< double >("lead-time"));
        }
        if (node->Has("trail-time"))
        {
            SetTrailTime(node->GetValue< double >("trail-time"));
        }

        return true;
    }

    bool AddTrack( KTProcessedTrackData& trackData, unsigned component )
    {
        KTDataPtr ptr( new KTDataPtr() );
        KTSpectrumCollectionData* newWaterfall = &ptr->Of< KTSpectrumCollectionData >();

        newWaterfall->SetStartTime( trackData.fStartTimeInRunC );
        newWaterfall->SetEndTime( trackData.fEndTimeInRunC );
        newWaterfall->SetFilling( false );

        fWaterfallSets[component].insert( std::make_pair< KTDataPtr, KTSpectrumCollectionData* >( &ptr, newWaterfall ) );

        return true;
    }

    bool ConsiderSpectrum( KTPowerSpectrum& ps, KTSliceHeader& slice, unsigned component )
    {
        for( std::set< std::pair< KTDataPtr, KTPSCollectionData* >, KTTrackCompare >::const_iterator it = fWaterfallSets[component].begin(); it != fTracks[component].end(); ++it )
        {
            if( slice.fTimeInRun >= it->second->fStartTime - fLeadTime && slice.fTimeInRun <= it->second->fEndTime + fTrailTime )
            {
                it->second->AddSpectrum( slice.fTimeInRun, ps );
                it->second->SetDeltaT( slice.fSliceLength );
                it->second->SetFilling( true );
            }
            else
            {
                if( it->second->GetFilling() )
                {
                    it->second->SetFilling( false );
                    FinishSC( it->first );
                    fWaterfallSets[component].erase( it );
                }
                else
                    it->second->SetFilling( false );
            }
        }

        return true;
    }

    bool FinishSC( KTDataPtr data )
    {
        fWaterfallSignal( data );
    }

    bool KTSpectrogramCollector::ReceiveTrack( KTProcessedTrackData& data )
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrum(0)->FindBin(fMinFrequency));
            KTDEBUG(sdlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrum(0)->FindBin(fMaxFrequency));
            KTDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        }

        if( !AddTrack( data, data.fComponent ) )
        {
            KTERROR(sdlog, "Spectrogram collection could not add track! (component " << data.fComponent << ")" );
        }

        return true;
    }
    
    bool KTSpectrogramCollector::ReceiveSpectrum( KTPowerSpectrumData& data, KTSliceHeader& sliceData )
    {
        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectrum(0)->FindBin(fMinFrequency));
            KTDEBUG(sdlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectrum(0)->FindBin(fMaxFrequency));
            KTDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        }

        unsigned nComponents = data.GetNComponents();

        for (unsigned iComponent=0; iComponent<nComponents; ++iComponent)
        {
            if (! ConsiderSpectrum(data.GetSpectrum(iComponent), iComponent))
            {
                KTERROR(sdlog, "Spectrogram collector could not receive spectrum! (component " << iComponent << ")");
                return false;
            }
        }

        return true;
    }
