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
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTProcessedTrackData.hh"
#include "KTMath.hh"

#include <cmath>

namespace Katydid
{
    KTLOGGER(evlog, "KTQuadraticPhaseShift");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTQuadraticPhaseShift, "quadratic-phase");

    KTQuadraticPhaseShift::KTQuadraticPhaseShift(const std::string& name) :
            KTProcessor(name),
            fSlope(0.),
            fTSSignal("ts", this),
            fFSSignal("fs", this),
            fProcTrackSlot("track", this, &KTQuadraticPhaseShift::AssignPhase)
    {
        RegisterSlot( "ts", this, &KTQuadraticPhaseShift::SlotFunctionTS );
        RegisterSlot( "fs", this, &KTQuadraticPhaseShift::SlotFunctionFS );
    }

    KTQuadraticPhaseShift::~KTQuadraticPhaseShift()
    {
    }

    bool KTQuadraticPhaseShift::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSlope(node->get_value< double >("slope", fSlope));

        return true;
    }

    bool KTQuadraticPhaseShift::ProcessTimeSeries( KTTimeSeriesData& tsData, KTTimeSeriesData& newData, KTSliceHeader& slice )
    {
        KTDEBUG(evlog, "Receiving time series for quadratic phase shift");

        KTTimeSeriesFFTW* ts = nullptr;   // time series from data
        KTTimeSeriesFFTW* newTS = new KTTimeSeriesFFTW( slice.GetSliceSize(), 0.0, (double)slice.GetSliceLength() );

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
            
            // Loop through all bins
            for( unsigned iBin = 0; iBin < ts->GetNTimeBins(); ++iBin )
            {
                // Obtain norm and phase from components
                norm = std::hypot( (*ts)(iBin)[0], (*ts)(iBin)[1] );
                phase = atan2( (*ts)(iBin)[1], (*ts)(iBin)[0] );

                // Shift phase
                phase -= q * KTMath::Pi() / (double)(slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize());

                // Assign components from norm and new phase
                (*newTS)(iBin)[0] = norm * cos( phase );
                (*newTS)(iBin)[1] = norm * sin( phase );

                //KTDEBUG( evlog, "Old bin: " << (*ts)(iBin)[0] << " + " << (*ts)(iBin)[1] << "j" );
                //KTDEBUG( evlog, "New bin: " << (*newTS)(iBin)[0] << " + " << (*newTS)(iBin)[1] << "j" );
            }

            newData.SetTimeSeries( newTS, iComponent );
        }

        KTDEBUG(evlog, "Successfully shifted phase");

        return true;
    }

    bool KTQuadraticPhaseShift::ProcessFrequencySpectrum( KTFrequencySpectrumDataFFTW& fsData, KTFrequencySpectrumDataFFTW& newData, KTSliceHeader& slice )
    {
        KTDEBUG(evlog, "Receiving frequency spectrum for quadratic phase shift");

        KTFrequencySpectrumFFTW* fs = nullptr;   // frequency spectrum from data
        KTFrequencySpectrumFFTW* newFS = new KTFrequencySpectrumFFTW( slice.GetSliceSize(), 0.0, (double)slice.GetSampleRate() );

        double norm = 0.;                       // norm of the current FS value
        double phase = 0.;                      // argument of current FS value
        double q = fSlope;                      // q-value to determine phase shift

        KTINFO(evlog, "Set up frequency spectrum and slice parameters.");

        for( unsigned iComponent = 0; iComponent < fsData.GetNComponents(); ++iComponent )
        {
            KTDEBUG(evlog, "Processing component: " << iComponent);

            // get TS from data object and make the new TS
            fs = dynamic_cast< KTFrequencySpectrumFFTW* >(fsData.GetSpectrumFFTW( iComponent ));
            if( fs == nullptr )
            {
                KTWARN(evlog, "Couldn't find frequency spectrum object. Continuing to next component");
                continue;
            }
            
            // Loop through all bins
            for( unsigned iBin = 0; iBin < fs->GetNFrequencyBins(); ++iBin )
            {
                // Obtain norm and phase from components
                norm = std::hypot( (*fs)(iBin)[0], (*fs)(iBin)[1] );
                phase = atan2( (*fs)(iBin)[1], (*fs)(iBin)[0] );

                // Shift phase
                phase -= q * KTMath::Pi() / (double)(slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize()) * (double)(iBin - 0.5 * slice.GetSliceSize());

                // Assign components from norm and new phase
                (*newFS)(iBin)[0] = norm * cos( phase );
                (*newFS)(iBin)[1] = norm * sin( phase );
            }

            newData.SetSpectrum( newFS, iComponent );
        }

        KTDEBUG(evlog, "Successfully shifted phase");

        return true;
    }

    bool KTQuadraticPhaseShift::AssignPhase( KTProcessedTrackData& trackData )
    {
        SetSlope( trackData.GetSlope() * KTMath::Pi() );
        KTINFO(evlog, "Set q-value: " << GetSlope());

        return true;
    }

    void KTQuadraticPhaseShift::SlotFunctionTS( Nymph::KTDataPtr data )
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

    void KTQuadraticPhaseShift::SlotFunctionFS( Nymph::KTDataPtr data )
    {
        if (! data->Has< KTFrequencySpectrumDataFFTW >())
        {
            KTERROR(evlog, "Data not found with type < KTFrequencySpectrumDataFFTW >!");
            return;
        }

        if (! data->Has< KTSliceHeader >())
        {
            KTERROR(evlog, "Data not found with type < KTSliceHeader >!");
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

        KTFrequencySpectrumDataFFTW& newFS = newData->Of< KTFrequencySpectrumDataFFTW >();

        if( ! ProcessFrequencySpectrum( data->Of< KTFrequencySpectrumDataFFTW >(), newFS, newSlc ) )
        {
            KTERROR(evlog, "Something went wrong with the chirp transform");
        }

        KTDEBUG(evlog, "Emitting FS signal");
        fFSSignal( newData );
    }

} // namespace Katydid
