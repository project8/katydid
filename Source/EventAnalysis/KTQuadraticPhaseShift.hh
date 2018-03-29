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


namespace Katydid
{

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
     - "ts": void (Nymph::KTDataPtr) -- Emitted upon successful time series processing; Guarantees KTTimeSeriesData and KTSliceHeader
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
            Nymph::KTSlotDataTwoTypes< KTTimeSeriesData, KTSliceHeader > fTSSlot;
            Nymph::KTSlotDataOneType< KTProcessedTrackData > fProcTrackSlot;

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
}

#endif /* KTQUADRATICPHASESHIFT_HH_ */
