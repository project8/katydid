/*
 * KTQuadraticPhaseShift.cc
 *
 *  Created on: Jan 17, 2017
 *      Author: ezayas
 */

#include "KTQuadraticPhaseShift.hh"

#include "KTLogger.hh"

#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTProcessedTrackData.hh"
#include "KTMath.hh"

#include <cmath>

namespace Katydid
{
    KTLOGGER(evlog, "KTQuadraticPhaseShift");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTQuadraticPhaseShift, "quadratic-phase");

    KTQuadraticPhaseShift::KTQuadraticPhaseShift(const std::string& name) :
            KTProcessor(name),
            fSlope(0.),
            fTSSignal("time-series", this)
    {
        RegisterSlot( "ts", this, &KTQuadraticPhaseShift::SlotFunctionTimeSeries );
        RegisterSlot( "track", this, &KTQuadraticPhaseShift::SlotFunctionTrack );
    }

    KTQuadraticPhaseShift::~KTQuadraticPhaseShift()
    {
    }

    bool KTQuadraticPhaseShift::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetSlope(node->get_value< double >("slope", fSlope));

        return true;
    }

    bool KTQuadraticPhaseShift::ProcessTimeSeries( KTTimeSeriesData& tsData, KTSliceHeader& slice )
    {
        KTDEBUG(evlog, "Receiving time series for quadratic phase shift");

        // Make new KTDataPtr which will contain the processed KTTimeSeries
//        Nymph::KTDataPtr newData( new Nymph::KTData() );
//        KTTimeSeriesData* newTSData = &newData->Of< KTTimeSeriesData >();

        KTTimeSeriesFFTW* ts;           // time series after phase shift
        KTTimeSeriesFFTW* tsFromData;   // time series from data, before phase shift

        // Slice and TS parameters

        double t = slice.GetTimeInAcq();    // time value of the current bin
        double dt = slice.GetBinWidth();    // time step
        double norm = 0.;                   // norm of the current TS value
        double phase = 0.;                  // argument of current TS value
        double q = GetSlope();              // q-value to determine phase shift

        KTINFO(evlog, "Set up time series and slice parameters.");
        KTDEBUG(evlog, "Initial t = " << t);
        KTDEBUG(evlog, "Time step = " << dt);
        KTDEBUG(evlog, "Slope = " << q);
        
        for( unsigned iComponent = 0; iComponent < tsData.GetNComponents(); ++iComponent )
        {
            KTDEBUG(evlog, "Processing component: " << iComponent);

            // get TS from data object and make the new TS
            tsFromData = dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries( iComponent ));
            ts = new KTTimeSeriesFFTW( tsFromData->GetNTimeBins(), slice.GetTimeInAcq(), slice.GetTimeInAcq() + slice.GetSliceLength() );

            // Loop through all bins
            for( unsigned iBin = 0; iBin < tsFromData->GetNTimeBins(); ++iBin )
            {
                // Obtain norm and phase from components
                norm = sqrt( (*tsFromData)(iBin)[0] * (*tsFromData)(iBin)[0] + (*tsFromData)(iBin)[1] * (*tsFromData)(iBin)[1] );
                phase = atan2( (*tsFromData)(iBin)[1], (*tsFromData)(iBin)[0] );

                // Shift phase
                phase -= q * t * t;

                // Assign components from norm and new phase
                (*tsFromData)(iBin)[0] = norm * cos( phase );
                (*tsFromData)(iBin)[1] = norm * sin( phase );

                // Increment time value
                t += dt;
            }

            // Fill KTTimeSeriesData object with the new TS
//            newTSData->SetTimeSeries( ts, iComponent );
        }

        // We should also preserve the slice header in the new KTDataPtr
//        KTSliceHeader* newSlice = &newData->Of< KTSliceHeader >();
//        *newSlice = slice;

        // Emit signal
//        fTSSignal( newData );

        return true;
    }

    bool KTQuadraticPhaseShift::AssignPhase( KTProcessedTrackData& trackData )
    {
        SetSlope( trackData.GetSlope() * KTMath::Pi() );
        KTINFO(evlog, "Set q-value: " << GetSlope());

        return true;
    }

} // namespace Katydid
