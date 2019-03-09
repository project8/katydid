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
            fSlope(0.),
            fInitialized(false),
            fForwardFFT(),
            fReverseFFT(),
            fTSSignal("ts", this),
            fFSSignal("fs", this)
    {
        RegisterSlot( "ts", this, &KTFractionalFFT::SlotFunctionTS );
    }

    KTFractionalFFT::~KTFractionalFFT()
    {
    }

    bool KTFractionalFFT::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSlope(node->get_value< double >("slope", fSlope));

        return true;
    }

    bool KTFractionalFFT::Initialize(unsigned s)
    {
        if( fInitialized )
        {
            return true;
        }

        fForwardFFT.SetComplexAsIQ( true );
        fForwardFFT.SetTimeSize(s);
        fForwardFFT.SetTransformFlag("ESTIMATE");
        fReverseFFT.SetTimeSize(s);
        fReverseFFT.SetTransformFlag("ESTIMATE");

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

    bool KTFractionalFFT::ProcessTimeSeries( KTTimeSeriesData& tsData, KTTimeSeriesData& newData, KTSliceHeader& slice )
    {
        KTDEBUG(evlog, "Receiving time series for quadratic phase shift");

        KTTimeSeriesFFTW* ts = nullptr;   // time series from data

        double norm = 0.;                       // norm of the current TS value
        double phase = 0.;                      // argument of current TS value
        double q = fSlope;                      // q-value to determine phase shift

        KTINFO(evlog, "Set up time series and slice parameters. q = " << q);
        
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
                phase -= q * KTMath::Pi() / (double)(slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize());

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
                phase -= q * KTMath::Pi() / (double)(slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize());

                // Assign components from norm and new phase
                (*ts)(iBin)[0] = norm * cos( phase );
                (*ts)(iBin)[1] = norm * sin( phase );
            }

            // Reverse FFT
            KTTimeSeriesFFTW* newTS = fReverseFFT.TransformToComplex( fs );

            // Third chirp transform
            for( unsigned iBin = 0; iBin < newTS->GetNTimeBins(); ++iBin )
            {
                // Obtain norm and phase from components
                norm = std::hypot( (*newTS)(iBin)[0], (*newTS)(iBin)[1] );
                phase = atan2( (*newTS)(iBin)[1], (*newTS)(iBin)[0] );

                // Shift phase
                phase -= q * KTMath::Pi() / (double)(slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize());

                // Assign components from norm and new phase
                (*newTS)(iBin)[0] = norm * cos( phase );
                (*newTS)(iBin)[1] = norm * sin( phase );
            }

            newData.SetTimeSeries( newTS, iComponent );
            
        }

        KTDEBUG(evlog, "Successfully shifted phase");

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

        KTTimeSeriesData& newTS = newData->Of< KTTimeSeriesData >();

        if( ! ProcessTimeSeries( data->Of< KTTimeSeriesData >(), newTS, newSlc ) )
        {
            KTERROR(evlog, "Something went wrong with the chirp transform");
        }

        KTDEBUG(evlog, "Emitting TS signal");
        fTSSignal( newData );
    }

} // namespace Katydid
