/*
 * KTFractionalFFT.cc
 *
 *  Created on: Jan 17, 2017
 *      Author: ezayas
 */

#include "KTFractionalFFT.hh"

#include "KTLogger.hh"

#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTProcessedTrackData.hh"
#include "KTMath.hh"

#include <cmath>

namespace Katydid
{
    KTLOGGER(evlog, "KTFractionalFFT");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTFractionalFFT, "fractional-fft");

    KTFractionalFFT::KTFractionalFFT(const std::string& name) :
            KTProcessor(name),
            fAlpha(0.),
            fInitialized(false),
            fForwardFFT(),
            fReverseFFT(),
            fTSFSSignal("ts-and-fs", this)
    {
        RegisterSlot( "ts", this, &KTFractionalFFT::SlotFunctionTS );
    }

    KTFractionalFFT::~KTFractionalFFT()
    {
    }

    bool KTFractionalFFT::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetAlpha(node->get_value< double >("alpha", fAlpha));

        return true;
    }

    bool KTFractionalFFT::Initialize(unsigned s)
    {
        if( fInitialized )
        {
            return true;
        }

        fForwardFFT.SetComplexAsIQ( true );
        fForwardFFT.SetTimeSize( s );
        fForwardFFT.SetTransformFlag( "ESTIMATE" );
        fReverseFFT.SetTimeSize( s );
        fReverseFFT.SetTransformFlag( "ESTIMATE" );

        if (! fForwardFFT.InitializeForComplexTDD())
        {
            KTERROR(evlog, "Error while initializing the forward FFT");
            return false;
        }

        if (! fReverseFFT.InitializeForComplexTDD())
        {
            KTERROR(evlog, "Error while initializing the reverse FFT");
            return false;
        }

        return true;
    }

    bool KTFractionalFFT::ProcessTimeSeries( KTTimeSeriesData& tsData, KTTimeSeriesData& newTSData, KTFrequencySpectrumDataFFTW& newFSData, KTSliceHeader& slice )
    {
        KTDEBUG(evlog, "Receiving time series for fractional FFT");

        KTTimeSeriesFFTW* ts = nullptr;   // time series from data

        KTINFO(evlog, "q1 N/pi = " << tan( 0.5 * fAlpha ));
        KTINFO(evlog, "q2 N/pi = " << sin( fAlpha ));

        double norm = 0.;                       // norm of the current TS value
        double phase = 0.;                      // argument of current TS value
        double q1 = tan( 0.5 * fAlpha ) * KTMath::Pi() / (double)(slice.GetSliceSize());
        double q2 = sin( fAlpha ) * KTMath::Pi() / (double)(slice.GetSliceSize());

        for( unsigned iComponent = 0; iComponent < tsData.GetNComponents(); ++iComponent )
        {
            KTDEBUG(evlog, "Processing component: " << iComponent);

            // get TS from data object and make the new TS
            ts = dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries( iComponent ));
            if( ts == nullptr )
            {
                KTWARN(evlog, "Couldn't find time series object. Continuing to next component");
                continue;
            }
            
            // First chirp transform
            for( unsigned iBin = 0; iBin < ts->GetNTimeBins(); ++iBin )
            {
                // Obtain norm and phase from components
                norm = std::hypot( (*ts)(iBin)[0], (*ts)(iBin)[1] );
                phase = atan2( (*ts)(iBin)[1], (*ts)(iBin)[0] );

                // Shift phase
                phase -= q1 * (double)(iBin - 0.5 * slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize());

                // Assign components from norm and new phase
                (*ts)(iBin)[0] = norm * cos( phase );
                (*ts)(iBin)[1] = norm * sin( phase );
            }

            // Forward FFT
            KTFrequencySpectrumFFTW* fs = fForwardFFT.Transform(ts);

            // Second chirp transform
            for( unsigned iBin = 0; iBin < fs->GetNFrequencyBins(); ++iBin )
            {
                // Obtain norm and phase from components
                norm = std::hypot( (*fs)(iBin)[0], (*fs)(iBin)[1] );
                phase = atan2( (*fs)(iBin)[1], (*fs)(iBin)[0] );

                // Shift phase
                phase -= q2 * (double)(iBin - 0.5 * slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize());

                // Assign components from norm and new phase
                (*ts)(iBin)[0] = norm * cos( phase );
                (*ts)(iBin)[1] = norm * sin( phase );
            }

            // Reverse FFT
            KTTimeSeriesFFTW* newTS = fReverseFFT.TransformToComplex( fs );
            KTFrequencySpectrumFFTW* newFS = new KTFrequencySpectrumFFTW( ts->GetNTimeBins(), 0.0, slice.GetSampleRate() );
            newFS->SetNTimeBins( ts->GetNTimeBins() );

            // Third chirp transform
            for( unsigned iBin = 0; iBin < newTS->GetNTimeBins(); ++iBin )
            {
                // Obtain norm and phase from components
                norm = std::hypot( (*newTS)(iBin)[0], (*newTS)(iBin)[1] );
                phase = atan2( (*newTS)(iBin)[1], (*newTS)(iBin)[0] );

                // Shift phase
                phase -= q1 * (double)(iBin - 0.5 * slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize());

                // Assign components from norm and new phase
                (*newTS)(iBin)[0] = norm * cos( phase );
                (*newTS)(iBin)[1] = norm * sin( phase );
                (*newFS)(iBin)[0] = norm * cos( phase );
                (*newFS)(iBin)[1] = norm * sin( phase );
            }

            newTSData.SetTimeSeries( newTS, iComponent );
            newFSData.SetSpectrum( newFS, iComponent );
        }

        return true;
    }

    void KTFractionalFFT::SlotFunctionTS( Nymph::KTDataPtr data )
    {
        if (! data->Has< KTTimeSeriesData >())
        {
            KTERROR(evlog, "Data not found with type < KTTimeSeriesData >!");
            return;
        }

        if (! data->Has< KTSliceHeader >())
        {
            KTERROR(evlog, "Data not found with type < KTSliceHeader >!");
            return;
        }

        if( ! Initialize( data->Of< KTSliceHeader >().GetSliceSize() ) )
        {
            KTERROR(evlog, "Something went wrong initializing the FFTs!");
            return;
        }

        Nymph::KTDataPtr newData( new Nymph::KTData() );
        KTSliceHeader& newSlc = newData->Of< KTSliceHeader >();
        KTSliceHeader& oldSlc = data->Of< KTSliceHeader >();

        newSlc.SetSliceSize( oldSlc.GetSliceSize() );
        newSlc.SetSampleRate( oldSlc.GetSampleRate() );
        newSlc.SetTimeInRun( oldSlc.GetTimeInRun() );
        newSlc.SetSliceLength( oldSlc.GetSliceLength() );
        newSlc.SetIsNewAcquisition( oldSlc.GetIsNewAcquisition() );

        if( ! ProcessTimeSeries( data->Of< KTTimeSeriesData >(), newData->Of< KTTimeSeriesData >(), newData->Of< KTFrequencySpectrumDataFFTW >(), newSlc ) )
        {
            KTERROR(evlog, "Something went wrong with the chirp transform");
        }

        KTDEBUG(evlog, "Emitting signal");
        fTSFSSignal( newData );
    }

} // namespace Katydid
