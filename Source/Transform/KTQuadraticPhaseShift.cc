/*
 * KTQuadraticPhaseShift.cc
 *
 *  Created on: Jan 17, 2017
 *      Author: ezayas
 */

#include "KTQuadraticPhaseShift.hh"

#include "KTLogger.hh"

#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTProcessedTrackData.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTEggHeader.hh"
#include "KTMath.hh"

#include <cmath>

namespace Katydid
{
    KTLOGGER(evlog, "KTQuadraticPhaseShift");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTQuadraticPhaseShift, "quadratic-phase");

    KTQuadraticPhaseShift::KTQuadraticPhaseShift(const std::string& name) :
            KTProcessor(name),
            fSlopeT(0.),
            fSlopeF(0.),
            fRotationAngle(0.),
            fTimeBinWidth(1.),
            fFrequencyBinWidth(1.),
            fMethod("dechirp"),
            fTSSignal("time-series", this),
            fFSSignal("frequency-spectrum", this)
    {
        RegisterSlot( "ts", this, &KTQuadraticPhaseShift::SlotFunctionTS );
        RegisterSlot( "fs", this, &KTQuadraticPhaseShift::SlotFunctionFS );
        RegisterSlot( "track", this, &KTQuadraticPhaseShift::SlotFunctionTrack );
        RegisterSlot( "header", this, &KTQuadraticPhaseShift::SlotFunctionHeader );
    }

    KTQuadraticPhaseShift::~KTQuadraticPhaseShift()
    {
    }

    bool KTQuadraticPhaseShift::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSlopeT(node->get_value< double >("slope", fSlopeT));
        SetRotationAngle(node->get_value< double >("angle", fRotationAngle));
        SetMethod(node->get_value< std::string >("method", fMethod));

        return true;
    }

    bool KTQuadraticPhaseShift::DechirpTS( KTTimeSeriesData& tsData, KTSliceHeader& slice )
    {
        KTDEBUG(evlog, "Receiving time series for quadratic phase shift");

        KTTimeSeriesFFTW* ts;   // time series from data

        // Slice and TS parameters

        double t = slice.GetTimeInAcq();    // time value of the current bin
        double dt = slice.GetBinWidth();    // time step
        double norm = 0.;                   // norm of the current TS value
        double phase = 0.;                  // argument of current TS value
        double q = GetSlopeT();             // q-value to determine phase shift

        KTINFO(evlog, "Set up time series and slice parameters.");
        KTDEBUG(evlog, "Initial t = " << t);
        KTDEBUG(evlog, "Time step = " << dt);
        KTDEBUG(evlog, "Slope = " << q);
        
        for( unsigned iComponent = 0; iComponent < tsData.GetNComponents(); ++iComponent )
        {
            KTDEBUG(evlog, "Processing component: " << iComponent);

            // get TS from data object and make the new TS
            ts = dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries( iComponent ));
            
            // Loop through all bins
            for( unsigned iBin = 0; iBin < ts->GetNTimeBins(); ++iBin )
            {
                // Obtain norm and phase from components
                norm = sqrt( (*ts)(iBin)[0] * (*ts)(iBin)[0] + (*ts)(iBin)[1] * (*ts)(iBin)[1] );
                phase = atan2( (*ts)(iBin)[1], (*ts)(iBin)[0] );

                // Shift phase
                phase -= q * t * t;

                // Assign components from norm and new phase
                (*ts)(iBin)[0] = norm * cos( phase );
                (*ts)(iBin)[1] = norm * sin( phase );

                // Increment time value
                t += dt;
            }
        }

        return true;
    }

    bool KTQuadraticPhaseShift::DechirpFS( KTFrequencySpectrumDataFFTW& fsData )
    {
        KTDEBUG(evlog, "Receiving frequency spectrum for quadratic phase shift");

        KTFrequencySpectrumFFTW* fs;   // frequency spectrum from data

        // Slice and TS parameters

        double f = 0;                               // frequency value of the current bin
        double df;                                  // frequency step
        double norm = 0.;                           // norm of the current FS value
        double phase = 0.;                          // argument of current FS value
        double q = GetSlopeF();                     // q-value to determine phase shift

        KTINFO(evlog, "Set up frequency spectrum parameters.");
  
        for( unsigned iComponent = 0; iComponent < fsData.GetNComponents(); ++iComponent )
        {
            KTDEBUG(evlog, "Processing component: " << iComponent);

            // get TS from data object and make the new TS
            fs = dynamic_cast< KTFrequencySpectrumFFTW* >(fsData.GetSpectrum( iComponent ));
            df = fs->GetFrequencyBinWidth();
            
            // Loop through all bins
            for( unsigned iBin = 0; iBin < fs->GetNFrequencyBins(); ++iBin )
            {
                // Obtain norm and phase from components
                norm = sqrt( (*fs)(iBin)[0] * (*fs)(iBin)[0] + (*fs)(iBin)[1] * (*fs)(iBin)[1] );
                phase = atan2( (*fs)(iBin)[1], (*fs)(iBin)[0] );

                // Shift phase
                phase -= q * f * f;

                // Assign components from norm and new phase
                (*fs)(iBin)[0] = norm * cos( phase );
                (*fs)(iBin)[1] = norm * sin( phase );

                // Increment time value
                f += df;
            }
        }

        return true;
    }

    bool KTQuadraticPhaseShift::AssignPhase( KTProcessedTrackData& trackData )
    {
        SetRotationAngle( atan( trackData.GetSlope() * fTimeBinWidth * fTimeBinWidth ) );
        KTINFO(evlog, "Set rotation angle: " << GetRotationAngle());

        if( GetMethod() == "dechirp" )
        {
            SetSlopeT( trackData.GetSlope() * KTMath::Pi() );
        }
        else if( GetMethod() == "frfft" )
        {
            SetSlopeT( tan( 0.5 * GetRotationAngle() ) * fFrequencyBinWidth * fFrequencyBinWidth );
            SetSlopeF( sin( GetRotationAngle() ) * fTimeBinWidth * fTimeBinWidth );
        }
        else
        {
            KTWARN(evlog, "Method not properly specified. Reverting to default value, \"dechirp\"");
            SetMethod( "dechirp" );
            SetSlopeT( trackData.GetSlope() * KTMath::Pi() );
        }

        return true;
    }

    bool KTQuadraticPhaseShift::InitializeFrFFT( KTEggHeader& header )
    {
        fTimeBinWidth = header.GetChannelHeader(0)->GetSliceSize();
        fFrequencyBinWidth = 1. / (double)fTimeBinWidth;

        return true;
    }

    void KTQuadraticPhaseShift::SlotFunctionTS( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTTimeSeriesData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTTimeSeriesData >!");
            return;
        }
        if (! data->Has< KTSliceHeader >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTSliceHeader >!");
            return;
        }

        // Use a copy of the original data pointer rather than alter it
        Nymph::KTDataPtr newData = data;

        // Call function
        if(! DechirpTS( newData->Of< KTTimeSeriesData >(), newData->Of< KTSliceHeader >() ))
        {
            KTERROR(avlog_hh, "Something went wrong while analyzing time series data!");
            return;
        }

        // Emit signal
        fTSSignal( newData );
    
        return;
    }

    void KTQuadraticPhaseShift::SlotFunctionFS( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTFrequencySpectrumDataFFTW >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTFrequencySpectrumDataFFTW >!");
            return;
        }

        // Use a new data pointer
        Nymph::KTDataPtr newData;

        // Copy the old frequency spectrum
        KTFrequencySpectrumDataFFTW& newFS = newData->Of< KTFrequencySpectrumDataFFTW >();
        newFS = data->Of< KTFrequencySpectrumDataFFTW >();

        // Call function
        if(! DechirpFS( newData->Of< KTFrequencySpectrumDataFFTW >() ))
        {
            KTERROR(avlog_hh, "Something went wrong while analyzing frequency spectrum data!");
            return;
        }

        // Emit signal
        fFSSignal( newData );
    
        return;
    } 

    void KTQuadraticPhaseShift::SlotFunctionTrack( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTProcessedTrackData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTProcessedTrackData >!");
            return;
        }

        if(! AssignPhase( data->Of< KTProcessedTrackData >() ))
        {
            KTERROR(avlog_hh, "Something went wrong setting the phase slope!");
            return;
        }
    }

    void KTQuadraticPhaseShift::SlotFunctionHeader( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTEggHeader >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTEggHeader >!");
            return;
        }

        if(! InitializeFrFFT( data->Of< KTEggHeader >() ))
        {
            KTERROR(avlog_hh, "Something went wrong initializing the fractional FFT!");
            return;
        }
    }

} // namespace Katydid
