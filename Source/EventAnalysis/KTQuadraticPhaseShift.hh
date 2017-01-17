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
     - "slope": double -- value of q, in Hz/s

     Slots:
     - "track": void (Nymph::KTDataPtr) -- Sets the value of q equal to the slope of a track; Requires KTProcessedTrackData; Adds nothing
     - "ts": void (Nymph::KTDataPtr) -- Multiplies a time series by the quadratic phase; Requires KTTimeSeriesData and KTSliceHeader; Adds nothing

     Signals:
     - "time-series": void (Nymph::KTDataPtr) -- Emitted upon successful time series processing; Guarantees KTTimeSeriesData and KTSliceHeader
    */

    class KTQuadraticPhaseShift : public Nymph::KTProcessor
    {
        public:
            KTQuadraticPhaseShift(const std::string& name = "quadratic-phase");
            virtual ~KTQuadraticPhaseShift();

            bool Configure(const scarab::param_node* node);

            double GetSlope() const;
            void SetSlope(double q);

        private:
            double fSlope;


        public:
            bool AssignPhase( KTProcessedTrackData& trackData );
            bool ProcessTimeSeries( KTTimeSeriesData& tsData, KTSliceHeader& slice );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTSSignal;

            //***************
            // Slots
            //***************

        private:
            void SlotFunctionTimeSeries( Nymph::KTDataPtr data );
            void SlotFunctionTrack( Nymph::KTDataPtr data );

    };

    inline double KTQuadraticPhaseShift::GetSlope() const
    {
        return fSlope;
    }

    inline void KTQuadraticPhaseShift::SetSlope(double q)
    {
        fSlope = q;
        return;
    }

    void KTQuadraticPhaseShift::SlotFunctionTimeSeries( Nymph::KTDataPtr data )
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

        // Call function
        if(! ProcessTimeSeries( data->Of< KTTimeSeriesData >(), data->Of< KTSliceHeader >() ))
        {
            KTERROR(avlog_hh, "Something went wrong while analyzing time seris data!");
            return;
        }
    
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
}

#endif /* KTQUADRATICPHASESHIFT_HH_ */
