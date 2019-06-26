/*
 * KTFractionalFFT.hh
 *
 *  Created on: Mar 8, 2019
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

     @brief Performs chirp transform and Fractional FFT via the Garcia algorithm

     @details
     Separate slots for the chirp transform and Fractional FFT, both of which take a time series data object. The Fractional FFT
     is performed via chirp -> FFT -> chirp -> Reverse FFT -> chirp, as in Garcia et al. Applied Optics 35, 35. 1996
     Track slope / rotation angle can be specified directly in the config, or by an incoming track
     The chirp-only transform is performed in-place. The Fractional FFT is not. The Fractional FFT uses a custom slot function
     to create a new data pointer, rather than append to the old one. This avoids complications with the ordinary FFT further in the
     analysis chain.

     Configuration name: "fracitonal-fft"

     Available configuration values:
     - "alpha": double -- rotation angle for fractional FFT, in radians
     - "slope": double -- track slope for chirp transform, in Hz/s
     - "transform-flag": string -- flag that determines how much planning is done prior to any transforms (see KTForwardFFTW.hh)

     Slots:
     - "track": void (Nymph::KTDataPtr) -- Sets the value of slope and alpha from a track; Requires KTProcessedTrackData; Adds nothing
     - "ts": void (Nymph::KTDataPtr) -- Performs fractional FFT on time series; Requires KTTimeSeriesData and KTSliceHeader; Adds KTFrequencySpectrumDataFFTW
     - "ts-chirp": void (Nymph::KTDataPtr) -- Performs chirp transform on time series; Requires KTTimeSeriesData and KTSliceHeader; Adds nothing
 
     Signals:
     - "ts": void (Nymph::KTDataPtr) -- Emitted upon successful chirp transform; Guarantees KTTimeSeriesData
     - "ts-and-fs": void (Nymph::KTDataPtr) -- Emitted upon successful time series processing; Guarantees KTTimeSeriesData, KTFrequencySpectrumDataFFTW and KTSliceHeader
    */

    class KTFractionalFFT : public Nymph::KTProcessor
    {
        public:
            KTFractionalFFT(const std::string& name = "fractional-fft");
            virtual ~KTFractionalFFT();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, Alpha);
            MEMBERVARIABLEREF_NOSET(bool, CalculateAlpha);
            MEMBERVARIABLE(double, Slope)
            MEMBERVARIABLE(bool, Initialized);
            MEMBERVARIABLE(std::string, TransformFlag);

        private:

            KTForwardFFTW fForwardFFT;
            KTReverseFFTW fReverseFFT;

        public:
            bool Initialize(unsigned s);
            bool ProcessTimeSeries( KTTimeSeriesData& tsData, KTTimeSeriesData& newTSData, KTFrequencySpectrumDataFFTW& newFSData, KTSliceHeader& slice );
            bool ProcessTimeSeriesChirpOnly( KTTimeSeriesData& tsData, KTSliceHeader& slice );
            bool AssignSlopeParams( KTProcessedTrackData& trackData );
            
            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTSFSSignal;
            Nymph::KTSignalData fTSSignal;

            //***************
            // Slots
            //***************
            Nymph::KTSlotDataTwoTypes< KTTimeSeriesData, KTSliceHeader > fChirpSlot;
            Nymph::KTSlotDataOneType< KTProcessedTrackData > fProcTrackSlot;

        private:
            void SlotFunctionTS( Nymph::KTDataPtr data );

    };
}

#endif /* KTFRACTIONALFFT_HH_ */
