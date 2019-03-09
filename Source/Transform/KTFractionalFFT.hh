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

     @brief Multiplies a time series by a quadratic phase exp( -i q t^2 ) where q is the slope of a track

     @details
     This processor performs the transformation (t, w) -> (t, w + q t) in time/frequency space by multiplying a quadratic phase
     by a time series. The value of q can be acquired from a track, or fixed in the config file (the latter should probably only
     be used for testing).

     Configuration name: "quadratic-phase"

     Available configuration values:
     - "slope": double -- value of q, in Hz/s

     Slots:
     - "track": void (Nymph::KTDataPtr) -- Sets the value of q equal to the slope of a track; Requires KTProcessedTrackData; Adds nothing
     - "ts": void (Nymph::KTDataPtr) -- Multiplies a time series by the quadratic phase; Requires KTTimeSeriesData and KTSliceHeader; Adds nothing
     - "fs": void (Nymph::KTDataPtr) -- Multiplies a frequency spectrum by the quadratic phase; Requires KTFrequencySpectrumDataFFTW and KTSliceHeader; Adds nothing

     Signals:
     - "ts": void (Nymph::KTDataPtr) -- Emitted upon successful time series processing; Guarantees KTTimeSeriesData and KTSliceHeader
    */

    class KTFractionalFFT : public Nymph::KTProcessor
    {
        public:
            KTFractionalFFT(const std::string& name = "fractional-fft");
            virtual ~KTFractionalFFT();

            bool Configure(const scarab::param_node* node);

            double GetSlope() const;
            void SetSlope(double q);

        private:
            double fSlope;
            bool fInitialized;

            KTForwardFFTW fForwardFFT;
            KTReverseFFTW fReverseFFT;

        public:
            bool Initialize(unsigned s);
            bool ProcessTimeSeries( KTTimeSeriesData& tsData, KTTimeSeriesData& newData, KTSliceHeader& slice );
            
            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTSSignal;
            Nymph::KTSignalData fFSSignal;

            //***************
            // Slots
            //***************

        private:
            void SlotFunctionTS( Nymph::KTDataPtr data );

    };

    inline double KTFractionalFFT::GetSlope() const
    {
        return fSlope;
    }

    inline void KTFractionalFFT::SetSlope(double q)
    {
        fSlope = q;
        return;
    }
}

#endif /* KTFRACTIONALFFT_HH_ */
