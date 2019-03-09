/*
 * KTFractionalFFT.hh
 *
 *  Created on: Jan 17, 2017
 *      Author: ezayas
 */

#ifndef KTFRACTIONALFFT_HH_
#define KTFRACTIONALFFT_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTSlot.hh"
#include "KTForwardFFTW.hh"
#include "KTReverseFFTW.hh"

namespace Katydid
{

    class KTSliceHeader;
    class KTProcessedTrackData;
    class KTTimeSeriesData;
    class KTFrequencySpectrumDataFFTW;

    /*
     @class KTFractionalFFT
     @author E. Zayas

     @brief [todo]

     @details
     [todo]

     Configuration name: "quadratic-phase"

     Available configuration values:
     - "alpha": double -- value of q, in Hz/s

     Slots:
     - "track": void (Nymph::KTDataPtr) -- Sets the value of q equal to the slope of a track; Requires KTProcessedTrackData; Adds nothing
     - "ts": void (Nymph::KTDataPtr) -- Multiplies a time series by the quadratic phase; Requires KTTimeSeriesData and KTSliceHeader; Adds nothing
 
     Signals:
     - "ts-and-fs": void (Nymph::KTDataPtr) -- Emitted upon successful time series processing; Guarantees KTTimeSeriesData, KTFrequencySpectrumDataFFTW and KTSliceHeader
    */

    class KTFractionalFFT : public Nymph::KTProcessor
    {
        public:
            KTFractionalFFT(const std::string& name = "fractional-fft");
            virtual ~KTFractionalFFT();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, Alpha);
            MEMBERVARIABLE(bool, Initialized);

        private:

            KTForwardFFTW fForwardFFT;
            KTReverseFFTW fReverseFFT;

        public:
            bool Initialize(unsigned s);
            bool ProcessTimeSeries( KTTimeSeriesData& tsData, KTTimeSeriesData& newTSData, KTFrequencySpectrumDataFFTW& newFSData, KTSliceHeader& slice );
            
            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTSFSSignal;

            //***************
            // Slots
            //***************

        private:
            void SlotFunctionTS( Nymph::KTDataPtr data );

    };
}

#endif /* KTFRACTIONALFFT_HH_ */
