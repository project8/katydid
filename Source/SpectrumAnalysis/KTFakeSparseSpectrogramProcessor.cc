/**
 @file KTFakeSparseSpectrogramProcessor.hh
 @brief Contains KTFakeSparseSpectrogramProcessor
 @details Generates a sparse spectrogram of uniform noise
 @author: E. Zayas
 @date: Sep 24, 2019
 */

#include "KTDiscriminatedPoints1DData.hh"
#include "KTLogger.hh"
#include "KTRandom.hh"
#include "KTSliceHeader.hh"

#include "KTFakeSparseSpectrogramProcessor.hh"

#include <random>

namespace Katydid
{
    KTLOGGER(sdlog, "KTFakeSparseSpectrogramProcessor");

    KT_REGISTER_PROCESSOR(KTFakeSparseSpectrogramProcessor, "fake-sparse-spectrogram-processor");

    KTFakeSparseSpectrogramProcessor::KTFakeSparseSpectrogramProcessor(const std::string& name) :
            KTPrimaryProcessor(name),
            fRunTime(1.0e-6),
            fSliceSize(1),
            fAcquisitionRate(1),
            fSNRThreshold(0.0),
            fProgInterval(1),
            fDisc1DSignal("disc-1d", this),
            fEggDoneSignal("egg-done", this)
    {
    }

    KTFakeSparseSpectrogramProcessor::~KTFakeSparseSpectrogramProcessor()
    {
    }

    bool KTFakeSparseSpectrogramProcessor::Configure(const scarab::param_node* node)
    {
        fRunTime = node->get_value< double >( "run-time", fRunTime );
        fSliceSize = node->get_value< unsigned >( "slice-size", fSliceSize );
        fAcquisitionRate = node->get_value< unsigned >( "acquisition-rate", fAcquisitionRate );
        fSNRThreshold = node->get_value< double >( "snr-threshold", fSNRThreshold );
        fProgInterval = node->get_value< unsigned >( "progress-report-interval", fProgInterval );

        return true;
    }


    bool KTFakeSparseSpectrogramProcessor::ProcessFakeSparseSpectrogram()
    {
        int nSlices = fRunTime * fAcquisitionRate / (double)fSliceSize;
        double timeBinWidth = (double)fSliceSize / (double)fAcquisitionRate;
        double freqBinWidth = fAcquisitionRate / (double)fSliceSize;

        KTPROG( sdlog, "Running false event simulation for " << fRunTime << " seconds" );
        KTPROG( sdlog, "Total slices: " << nSlices );        

        // Random engines
        KTRNGEngine* engine = KTGlobalRNGEngine::get_instance();
        std::random_device rd;
        engine->SetSeed( rd() );

        KTRNGUniform01<> uniformRandom;
        KTRNGPoisson<> poissonRandom;

        // Create fake data for every slice and run stf
        for( unsigned iSlice = 0; iSlice < nSlices; ++iSlice )
        {
            if( iSlice % fProgInterval == 0 )
            {
                KTPROG( sdlog, "Processing slice: " << iSlice << " / " << nSlices );
            }

            Nymph::KTDataPtr data( new Nymph::KTData() );
            KTSliceHeader& header = data->Of< KTSliceHeader >();
            KTDiscriminatedPoints1DData& disc1d = data->Of< KTDiscriminatedPoints1DData >();
            
            // Slice header
            header.SetBinWidth( timeBinWidth );
            header.SetTimeInAcq( timeBinWidth * (double)iSlice );
            header.SetTimeInRun( timeBinWidth * (double)iSlice);
            header.SetSampleRate( fAcquisitionRate );
            header.SetRawSliceSize( fSliceSize );
            header.SetAcquisitionID( 5 );

            // Points
            int nPoints = poissonRandom( exp( -1.0 * fSNRThreshold ) * (double)fSliceSize );
            for( unsigned iPoint = 0; iPoint < nPoints; ++iPoint )
            {
                double power = -1.0 * log( uniformRandom() ) + fSNRThreshold;
                double iBin = uniformRandom() * (double)fSliceSize;

                disc1d.AddPoint( (int)iBin, KTDiscriminatedPoints1DData::Point( freqBinWidth * (iBin + 0.5), power, fSNRThreshold, 1.0, 1.0, 1.0 ), 0 );
            }

            // Emit signal
            fDisc1DSignal( data );           
        }

        // Egg done signal
        fEggDoneSignal();

        KTPROG( sdlog, "Finished!" );

        return true;
    }

} /* namespace Katydid */
