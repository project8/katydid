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
            fTSSignal("ts", this),
            fTSSlot("ts", this, &KTQuadraticPhaseShift::ProcessTimeSeries, &fTSSignal),
            fProcTrackSlot("track", this, &KTQuadraticPhaseShift::AssignPhase)
    {
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

        KTTimeSeriesFFTW* ts = nullptr;   // time series from data

        // Slice and TS parameters

        double time = slice.GetTimeInAcq();     // time value of the current bin
        double timeStep = slice.GetBinWidth();  // time step
        double norm = 0.;                       // norm of the current TS value
        double phase = 0.;                      // argument of current TS value
        double q = fSlope;                      // q-value to determine phase shift

        KTINFO(evlog, "Set up time series and slice parameters.");
        KTDEBUG(evlog, "Initial t = " << time);
        KTDEBUG(evlog, "Time step = " << timeStep);
        KTDEBUG(evlog, "Slope = " << q);
        
        for( unsigned iComponent = 0; iComponent < tsData.GetNComponents(); ++iComponent )
        {
            KTDEBUG(evlog, "Processing component: " << iComponent);

            // get TS from data object and make the new TS
            ts = dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries( iComponent ));
            if( ts == nullptr )
            {
                KTWARN(evlog, "Couldn't find time series object. Continuing to next component");
                continue;
            }
            
            // Loop through all bins
            for( unsigned iBin = 0; iBin < ts->GetNTimeBins(); ++iBin )
            {
                // Obtain norm and phase from components
                norm = std::hypot( (*ts)(iBin)[0], (*ts)(iBin)[1] );
                phase = atan2( (*ts)(iBin)[1], (*ts)(iBin)[0] );

                // Shift phase
                phase -= q * time * time;

                // Assign components from norm and new phase
                (*ts)(iBin)[0] = norm * cos( phase );
                (*ts)(iBin)[1] = norm * sin( phase );

                // Increment time value
                time += timeStep;
            }
        }

        return true;
    }

    bool KTQuadraticPhaseShift::AssignPhase( KTProcessedTrackData& trackData )
    {
        SetSlope( trackData.GetSlope() * KTMath::Pi() );
        KTINFO(evlog, "Set q-value: " << GetSlope());

        return true;
    }

} // namespace Katydid
