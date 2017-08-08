/*
 * KTQuadraticPhaseShift.hh
 *
 *  Created on: Jan 17, 2017
 *      Author: ezayas
 */

#ifndef KTQUADRATICPHSAESHIFT_HH_
#define KTQUADRATICPHSAESHIFT_HH_

#include "KTProcessor.hh"
#include "KTData.hh"

#include "KTSlot.hh"
#include "KTLogger.hh"


namespace Katydid
{
    
    KTLOGGER(avlog_hh, "KTQuadraticPhaseShift.hh");

    class KTSliceHeader;
    class KTEggHeader;
    class KTFrequencySpectrumDataFFTW;
    class KTProcessedTrackData;
    class KTTimeSeriesData;

    /*
     @class KTQuadraticPhaseShift
     @author E. Zayas

     @brief Multiplies a time series by a quadratic phase exp( -i q t^2 ) where q is the slope of a track

     @details
     This processor performs the transformation (t, w) -> (t, w + q t) in time/frequency space by multiplying a quadratic phase
     by a time series. The value of q can be acquired from a track, or fixed in the config file (the latter should probably only
     be used for testing).

     Configuration name: "quadratic-phase"

     Available configuration values:
     - "slope": double -- value of q, in Hz/s. For simple de-chirp only
     - "angle": double -- value of rotation angle for FrFFT

     Slots:
     - "track": void (Nymph::KTDataPtr) -- Sets the value of q equal to the slope of a track; Requires KTProcessedTrackData; Adds nothing
     - "ts-dechirp": void (Nymph::KTDataPtr) -- Multiplies a time series by the quadratic phase; Requires KTTimeSeriesData and KTSliceHeader; Adds nothing
     - "ts-frfft": void (Nymph::KTDataPtr) -- Performs Fractional FFT; Requires KTTimeSeriesData and KTSliceHeader; Adds nothing

     Signals:
     - "time-series": void (Nymph::KTDataPtr) -- Emitted upon successful time series processing; Guarantees KTTimeSeriesData and KTSliceHeader
    */

    class KTQuadraticPhaseShift : public Nymph::KTProcessor
    {
        public:
            KTQuadraticPhaseShift(const std::string& name = "quadratic-phase");
            virtual ~KTQuadraticPhaseShift();

            bool Configure(const scarab::param_node* node);

            double GetSlopeT() const;
            void SetSlopeT(double q1);

            double GetSlopeF() const;
            void SetSlopeF(double q2);

            double GetRotationAngle() const;
            void SetRotationAngle(double alpha);

            std::string GetMethod() const;
            void SetMethod(std::string method);

        private:
            double fSlopeT;
            double fSlopeF;
            double fRotationAngle;
            std::string fMethod;
        
            double fTimeBinWidth;
            double fFrequencyBinWidth;

        public:
            bool AssignPhase( KTProcessedTrackData& trackData );
            bool DechirpTS( KTTimeSeriesData& tsData, KTSliceHeader& slice );
            bool DechirpFS( KTFrequencySpectrumDataFFTW& fsData );
            bool InitializeFrFFT( KTEggHeader& header );

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
            void SlotFunctionFS( Nymph::KTDataPtr data );
            void SlotFunctionTrack( Nymph::KTDataPtr data );
            void SlotFunctionHeader( Nymph::KTDataPtr data );

    };

    inline double KTQuadraticPhaseShift::GetSlopeT() const
    {
        return fSlopeT;
    }

    inline void KTQuadraticPhaseShift::SetSlopeT(double q1)
    {
        fSlopeT = q1;
        return;
    }

    inline double KTQuadraticPhaseShift::GetSlopeF() const
    {
        return fSlopeF;
    }

    inline void KTQuadraticPhaseShift::SetSlopeF(double q2)
    {
        fSlopeF = q2;
        return;
    }

    inline double KTQuadraticPhaseShift::GetRotationAngle() const
    {
        return fRotationAngle;
    }

    inline void KTQuadraticPhaseShift::SetRotationAngle(double alpha)
    {
        fRotationAngle = alpha;
        return;
    }

    inline std::string KTQuadraticPhaseShift::GetMethod() const
    {
        return fMethod;
    }

    inline void KTQuadraticPhaseShift::SetMethod(std::string method)
    {
        fMethod = method;
        return;
    }
    
}

#endif /* KTQUADRATICPHASESHIFT_HH_ */
