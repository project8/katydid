/**
 @file KTFakeSparseSpectrogramProcessor.hh
 @brief Contains KTFakeSparseSpectrogramProcessor
 @details Generates a sparse spectrogram of uniform noise
 @author: E. Zayas
 @date: Sep 24, 2019
 */

#ifndef KTFAKESPARSESPECTROGRAMPROCESSOR_HH_
#define KTFAKESPARSESPECTROGRAMPROCESSOR_HH_


#include "KTData.hh"
#include "KTSlot.hh"
#include "KTPrimaryProcessor.hh"

namespace Katydid
{
    /*!
     @class KTFakeSparseSpectrogramProcessor
     @author E. Zayas

     @brief Generates a sparse spectrogram of uniform noise

     @details
     Generates a sparse spectrogram of uniform noise

     Configuration name: "fake-sparse-spectrogram-processor"

     Available configuration options: (all required except the last)
     - "run-time": double -- Time to simulate (in seconds)
     - "slice-size": unsigned -- Size of the time series
     - "acquisition-rate": unsigned -- Samples per second. With slice size and runtime determines number of slices.
     - "snr-threshold": double -- Exponential suppression factor; determines density of points
     - "progress-report-interval": unsigned -- Interval (# of slices) between 
        reporting progress (mainly relevant for RELEASE builds); turn off by
        setting to 0

     Signals:
     - "disc-1d": void (Nymph::KTDataPtr) Emitted with each simulated slice; Guarantees KTDiscriminatedPoints1DData
     - "egg-done": void () Emitted after the final slice.
    
    */
    class KTFakeSparseSpectrogramProcessor : public Nymph::KTPrimaryProcessor
    {
        public:
            KTFakeSparseSpectrogramProcessor(const std::string& name = "fake-sparse-spectrogram-processor");
            virtual ~KTFakeSparseSpectrogramProcessor();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE( double, RunTime );
            MEMBERVARIABLE( unsigned, SliceSize );
            MEMBERVARIABLE( unsigned, AcquisitionRate );
            MEMBERVARIABLE( double, SNRThreshold );
            MEMBERVARIABLE( unsigned, ProgInterval );

        public:
            bool Run();

            bool ProcessFakeSparseSpectrogram();


            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fDisc1DSignal;
            Nymph::KTSignalOneArg< void > fEggDoneSignal;

    };

    inline bool KTFakeSparseSpectrogramProcessor::Run()
    {
        return ProcessFakeSparseSpectrogram();
    }

} /* namespace Katydid */

#endif /* KTFAKESPARSESPECTROGRAMPROCESSOR_HH_ */
