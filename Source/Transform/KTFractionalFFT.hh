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
     This processor performs two different but related operations: the chirp transform and fractional FFT. They are accomplished through
     separate slot functions that each require a time series and slice header to be present.

     (a) The chirp transform shifts the phase of a time series by -q * t^2 where q is called the chirp rate. A new data pointer is emitted
         with the slice header and transformed time series data.

     (b) The fractional FFT is performed via chirp_1 -> FFT -> chirp_2 -> Reverse FFT -> chirp_1, as in Garcia et al. The journal reference is:
     Applied Optics 35, 35 (1996). This process uses two different chirp rates (denoted above by _1 and _2) that are both calculated from the
     rotation angle. The output is both a time series and fourier series with exactly the same data -- that is, they are not related by a
     transform but instead the bin content in each is identical. The domain of this fractional FFT output is a mix of time and frequency, so this allows
     the user to take advantage of either case.

     The chirp rate / rotation angle may be specified in one of two ways:
     - Extracted from a track, via the "track" slot. For both transforms, the parameter is calculated from the track slope.
     - Directly in the config, via "slope" for the chirp transform or "alpha" for the fractional FFT. In this case, neither parameter is calculated from
       the other so the correct one must be used for the desired transform. Use of the "track" slot calculates both parameters and thus overrides this option
       for either transform.

     Configuration name: "fractional-fft"

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
            bool ProcessTimeSeriesChirpOnly( KTTimeSeriesData& tsData, KTTimeSeriesData& newTSData, KTSliceHeader& slice );
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

            Nymph::KTSlotDataOneType< KTProcessedTrackData > fProcTrackSlot;

        private:
            void SlotFunctionTS( Nymph::KTDataPtr data );
            void SlotFunctionTSChirpOnly( Nymph::KTDataPtr data );

    };
}

#endif /* KTFRACTIONALFFT_HH_ */
