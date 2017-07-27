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

    inline void KTQuadraticPhaseShift::GetMethod() const
    {
        return fMethod;
    }

    inline void KTQuadraticPhaseShift::SetMethod(std::string method)
    {
        fMethod = method;
        return;
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
        newData = data->Of< KTFrequencySpectrumDataFFTW >();

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
}

#endif /* KTQUADRATICPHASESHIFT_HH_ */
