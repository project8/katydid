/*
 * KTFractionalFFT.cc
 *
 *  Created on: Mar 8, 2019
 *      Author: ezayas
 */

#include "KTFractionalFFT.hh"

#include "logger.hh"

#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTProcessedTrackData.hh"
#include "KTMath.hh"

#include <cmath>

namespace Katydid
{
    LOGGER(evlog, "KTFractionalFFT");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTFractionalFFT, "fractional-fft");

    KTFractionalFFT::KTFractionalFFT(const std::string& name) :
            KTProcessor(name),
            fAlpha(0.),
            fCalculateAlpha(false),
            fSlope(0.),
            fInitialized(false),
            fTransformFlag("ESTIMATE"),
            fForwardFFT(),
            fReverseFFT(),
            fProcTrackSlot("track", this, &KTFractionalFFT::AssignSlopeParams),
            fTSSignal("ts", this),
            fTSFSSignal("ts-and-fs", this)
    {
        RegisterSlot( "ts", this, &KTFractionalFFT::SlotFunctionTS );
        RegisterSlot( "ts-chirp", this, &KTFractionalFFT::SlotFunctionTSChirpOnly );
    }

    KTFractionalFFT::~KTFractionalFFT()
    {
    }

    bool KTFractionalFFT::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetAlpha(node->get_value< double >("alpha", fAlpha));
        SetSlope(node->get_value< double >("slope", fSlope));
        SetTransformFlag(node->get_value("transform-flag", fTransformFlag));

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
        fForwardFFT.SetTransformFlag( fTransformFlag );
        fReverseFFT.SetTimeSize( s );
        fReverseFFT.SetTransformFlag( fTransformFlag );

        if (! fForwardFFT.InitializeForComplexTDD())
        {
            LERROR(evlog, "Error while initializing the forward FFT");
            return false;
        }

        if (! fReverseFFT.InitializeForComplexTDD())
        {
            LERROR(evlog, "Error while initializing the reverse FFT");
            return false;
        }

        fInitialized = true;
        return true;
    }

    bool KTFractionalFFT::ProcessTimeSeriesChirpOnly( KTTimeSeriesData& tsData, KTTimeSeriesData& newTSData, KTSliceHeader& slice )
    {
        LDEBUG(evlog, "Receiving time series for fractional FFT");

        double binOffset = slice.GetTimeInAcq() / (double)slice.GetBinWidth();
        double chirpRate = fSlope * KTMath::Pi() * slice.GetBinWidth() * slice.GetBinWidth();

        for( unsigned iComponent = 0; iComponent < tsData.GetNComponents(); ++iComponent )
        {
            LDEBUG(evlog, "Processing component: " << iComponent);

            // Initialize vars
            //KTTimeSeriesFFTW* tsDup = new KTTimeSeriesFFTW( slice.GetSliceSize(), 0.0, slice.GetSliceLength() );

            // get TS from data object and make the new TS; remains owned by tsData
            KTTimeSeriesFFTW* ts = dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries( iComponent ));
            if( ts == nullptr )
            {
                LWARN(evlog, "Couldn't find time series object. Continuing to next component");
                continue;
            }
            
            //~ // Loop through all bins
            //~ for( unsigned iBin = 0; iBin < ts->GetNTimeBins(); ++iBin )
            //~ {
                //~ // Obtain norm and phase from components
                //~ norm = std::hypot( (*ts)(iBin)[0], (*ts)(iBin)[1] );
                //~ phase = atan2( (*ts)(iBin)[1], (*ts)(iBin)[0] );

                //~ // Shift phase
                //~ phase -= chirpRate * ((double)iBin + binOffset) * ((double)iBin + binOffset);

                //~ // Assign components from norm and new phase
                //~ (*tsDup)(iBin)[0] = norm * cos( phase );
                //~ (*tsDup)(iBin)[1] = norm * sin( phase );
            //~ }
            
            KTTimeSeriesFFTW* tsDup = new KTTimeSeriesFFTW( *ts ); //copy ts
            
            // Shift phase
            auto phase = -chirpRate * square(
                            Eigen::ArrayXd::LinSpaced(ts->GetNTimeBins(), 0, 
                                            ts->GetNTimeBins()-1) + binOffset);

            tsDup->GetData() *= exp(std::complex<double>{0.0, 1.0}*phase);
            
            newTSData.SetTimeSeries( tsDup, iComponent );  // tsDup now owned by newTSData
        }

        return true;
    }

    // This function should be split in smaller pieces!
    bool KTFractionalFFT::ProcessTimeSeries( KTTimeSeriesData& tsData, KTTimeSeriesData& newTSData, KTFrequencySpectrumDataFFTW& newFSData, KTSliceHeader& slice )
    {
        LDEBUG(evlog, "Receiving time series for fractional FFT");

        // Intermediate data objects that need to be initialized here
        KTTimeSeriesFFTW tsDup( slice.GetSliceSize(), 0.0, slice.GetSliceLength() );

        if( fCalculateAlpha )
        {
            fAlpha = atan2( fSlope * slice.GetBinWidth() * slice.GetBinWidth(), 1.0 );
            LINFO(evlog, "Calculated alpha = " << fAlpha);
        }

        // Chirp rates calculated from the rotation angle fAlpha according to the Garcia algorithm
        double q1 = tan( 0.5 * fAlpha ) * KTMath::Pi() / (double)(slice.GetSliceSize());
        double q2 = sin( fAlpha ) * KTMath::Pi() / (double)(slice.GetSliceSize());
        
        for( unsigned iComponent = 0; iComponent < tsData.GetNComponents(); ++iComponent )
        {
            LDEBUG(evlog, "Processing component: " << iComponent);  

            // get TS from data object and make the new TS; remains owned by tsData
            KTTimeSeriesFFTW* ts = dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries( iComponent ));
            if( ts == nullptr )
            {
                LWARN(evlog, "Couldn't find time series object. Continuing to next component");
                continue;
            }
            
            // First chirp transform
            //~ for( unsigned iBin = 0; iBin < ts->GetNTimeBins(); ++iBin )
            //~ {
                //~ // Obtain norm and phase from components
                //~ norm = std::hypot( (*ts)(iBin)[0], (*ts)(iBin)[1] );
                //~ phase = atan2( (*ts)(iBin)[1], (*ts)(iBin)[0] );

                //~ // Shift phase
                //~ phase -= q1 * (double)iBin * (double)iBin;

                //~ // Assign components from norm and new phase
                //~ (tsDup)(iBin)[0] = norm * cos( phase );
                //~ (tsDup)(iBin)[1] = norm * sin( phase );
            //~ }
            
            KTTimeSeriesFFTW tsDup{ *ts }; //copy ts
            
            // Shift phase
            auto phaseTs = -q1 * square(Eigen::ArrayXd::LinSpaced(
                                                    ts->GetNTimeBins(), 
                                                    0, 
                                                    ts->GetNTimeBins()-1));

            tsDup.GetData() *= exp(std::complex<double>{0.0, 1.0}*phaseTs);

            // Forward FFT
            KTFrequencySpectrumFFTW* fs = fForwardFFT.Transform( &tsDup );
            fs->SetNTimeBins( slice.GetSliceSize() );

            // Second chirp transform
            
            //~ for( unsigned iBin = 0; iBin < fs->GetNFrequencyBins(); ++iBin )
            //~ {
                //~ // Obtain norm and phase from components
                //~ norm = std::hypot( (*fs)(iBin)[0], (*fs)(iBin)[1] );
                //~ phase = atan2( (*fs)(iBin)[1], (*fs)(iBin)[0] );

                //~ // Shift phase
                //~ phase -= q2 * (double)iBin * (double)iBin;

                //~ // Assign components from norm and new phase
                //~ (*fs)(iBin)[0] = norm * cos( phase );
                //~ (*fs)(iBin)[1] = norm * sin( phase );
            //~ }
            
            // Shift phase
            auto phaseFs = -q2 * square(Eigen::ArrayXd::LinSpaced(
                                                    fs->GetNFrequencyBins(), 
                                                    0, 
                                                    fs->GetNFrequencyBins()-1));

            fs->GetData() *= exp(std::complex<double>{0.0, 1.0}*phaseFs);

            // Reverse FFT
            KTTimeSeriesFFTW* newTS = fReverseFFT.TransformToComplex( fs );
            KTFrequencySpectrumFFTW* newFS = new KTFrequencySpectrumFFTW( slice.GetSliceSize(), 0.0, slice.GetSampleRate() );
            newFS->SetNTimeBins( slice.GetSliceSize() );

            delete fs;  // cleanup of fs

            // Third chirp transform
            //~ for( unsigned iBin = 0; iBin < newTS->GetNTimeBins(); ++iBin )
            //~ {
                //~ // Obtain norm and phase from components
                //~ norm = std::hypot( (*newTS)(iBin)[0], (*newTS)(iBin)[1] );
                //~ phase = atan2( (*newTS)(iBin)[1], (*newTS)(iBin)[0] );

                //~ // Shift phase
                //~ phase -= q1 * (double)iBin * (double)iBin;

                //~ // Assign components from norm and new phase
                //~ (*newTS)(iBin)[0] = norm * cos( phase );
                //~ (*newTS)(iBin)[1] = norm * sin( phase );
                //~ (*newFS)(iBin) = std::polar(norm, phase); 
            //~ }
            
            // Shift phase
            auto phaseNewTS = -q1 * square(Eigen::ArrayXd::LinSpaced(
                                                    newTS->GetNTimeBins(), 
                                                    0, 
                                                    newTS->GetNTimeBins()-1));

            newTS->GetData() *= exp(std::complex<double>{0.0, 1.0}*phaseNewTS);

            newTSData.SetTimeSeries( newTS, iComponent );  // newTS now owned by newTSData
            newFSData.SetSpectrum( newFS, iComponent );  // newFS now owned by newFSData
        }

        return true;
    }

    void KTFractionalFFT::SlotFunctionTSChirpOnly( Nymph::KTDataPtr data )
    {
        if (! data->Has< KTTimeSeriesData >())
        {
            LERROR(evlog, "Data not found with type < KTTimeSeriesData >!");
            return;
        }

        if (! data->Has< KTSliceHeader >())
        {
            LERROR(evlog, "Data not found with type < KTSliceHeader >!");
            return;
        }

        if( ! Initialize( data->Of< KTSliceHeader >().GetSliceSize() ) )
        {
            LERROR(evlog, "Something went wrong initializing the FFTs!");
            return;
        }

        Nymph::KTDataPtr newData( new Nymph::KTData() );
        KTSliceHeader& newSlc = newData->Of< KTSliceHeader >();
        KTSliceHeader& oldSlc = data->Of< KTSliceHeader >();

        newSlc.CopySliceHeaderOnly( oldSlc );

        if( ! ProcessTimeSeriesChirpOnly( data->Of< KTTimeSeriesData >(), newData->Of< KTTimeSeriesData >(), newSlc ) )
        {
            LERROR(evlog, "Something went wrong with the chirp transform");
        }

        LDEBUG(evlog, "Emitting signal");
        fTSSignal( newData );
    }

    void KTFractionalFFT::SlotFunctionTS( Nymph::KTDataPtr data )
    {
        if (! data->Has< KTTimeSeriesData >())
        {
            LERROR(evlog, "Data not found with type < KTTimeSeriesData >!");
            return;
        }

        if (! data->Has< KTSliceHeader >())
        {
            LERROR(evlog, "Data not found with type < KTSliceHeader >!");
            return;
        }

        if( ! Initialize( data->Of< KTSliceHeader >().GetSliceSize() ) )
        {
            LERROR(evlog, "Something went wrong initializing the FFTs!");
            return;
        }

        Nymph::KTDataPtr newData( new Nymph::KTData() );
        KTSliceHeader& newSlc = newData->Of< KTSliceHeader >();
        KTSliceHeader& oldSlc = data->Of< KTSliceHeader >();

        newSlc.CopySliceHeaderOnly( oldSlc );

        if( ! ProcessTimeSeries( data->Of< KTTimeSeriesData >(), newData->Of< KTTimeSeriesData >(), newData->Of< KTFrequencySpectrumDataFFTW >(), newSlc ) )
        {
            LERROR(evlog, "Something went wrong with the chirp transform");
        }

        LDEBUG(evlog, "Emitting signal");
        fTSFSSignal( newData );
    }

    bool KTFractionalFFT::AssignSlopeParams( KTProcessedTrackData& trackData )
    {
        SetSlope( trackData.GetSlope() );
        fCalculateAlpha = true;

        LINFO(evlog, "Set chirp slope: " << GetSlope());

        return true;
    }

} // namespace Katydid
