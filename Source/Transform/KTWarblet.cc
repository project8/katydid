/*
 * KTWarblet.cc
 *
 *  Created on: Nov 20, 2017
 *      Author: buzinsky
 */

#include "KTWarblet.hh"

#include "KTLogger.hh"

#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTProcessedTrackData.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTEggHeader.hh"
#include "KTMath.hh"

#include <cmath>

namespace Katydid
{
    KTLOGGER(evlog, "KTWarblet");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTWarblet, "warblet");

    KTWarblet::KTWarblet(const std::string& name) :
            KTProcessor(name),
            fPitchAngle(0.),
            fAxialPhase(0.),
            fTimeBinWidth(1.),
            fFrequencyBinWidth(1.),
            fMethod("dewarble"),
            fTSSignal("time-series", this),
            fFSSignal("frequency-spectrum", this)
    {
        RegisterSlot( "ts", this, &KTWarblet::SlotFunctionTS );
        RegisterSlot( "header", this, &KTWarblet::SlotFunctionHeader );
    }

    KTWarblet::~KTWarblet()
    {
    }

    bool KTWarblet::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetPitchAngle(node->get_value< double >("pitch", fPitchAngle));
        SetAxialPhase(node->get_value< double >("axial_phase", fAxialPhase * KTMath::Pi() / 180.)); //Convert to radians immediately
        SetMethod(node->get_value< std::string >("method", fMethod));

        return true;
    }

    bool KTWarblet::DewarbleTS( KTTimeSeriesData& tsData, KTSliceHeader& slice )
    {
        KTDEBUG(evlog, "Receiving time series for dewarbling");

        KTTimeSeriesFFTW* ts;   // time series from data

        // Slice and TS parameters

        double t = slice.GetTimeInAcq();    // time value of the current bin
        double dt = slice.GetBinWidth();    // time step
        double norm = 0.;                   // norm of the current TS value
        double phase = 0.;                  // argument of current TS value

        const double tCharge = 1.602176e-19;
        const double tMass = 9.10938356e-31;
        const double tMassEV = 510.9989461e3;
        const double tC = 2.99792458e8;
        const double tEnergy = 17830.0;
        const double tGamma = 1. + tEnergy / tMassEV;
        const double tBeta = sqrt(1. - 1. / (tGamma * tGamma));
        const double tMuNull = 4. * KTMath::Pi() * 1e-7;

        //////////// Trap Parameters (Shouldn't be hardcoded!) ///////////
        double B0 = 0.9583;
        double tCoilRadius = 0.0084;
        double tCurrent = 1.001;

        double tMagneticFieldEffective = B0 - tMuNull * tCurrent / (2. * tCoilRadius);
        double L0 = tCoilRadius * sqrt((2. / 3. ) * (2. * tCoilRadius * B0 / (tMuNull * tCurrent) - 1. ));

        printf("L0: %e\n",L0);

        double tPitchAngle = GetPitchAngle();
        double tAxialPhase = GetAxialPhase();

        double tBetaZ0 = tBeta * cos(tPitchAngle);
        double tZMax = L0 * cos(tPitchAngle) / sin(tPitchAngle);
        double tAxialFrequency = tBetaZ0 * tC / tZMax;



        KTINFO(evlog, "Set up time series and slice parameters.");
        KTDEBUG(evlog, "Initial t = " << t);
        KTDEBUG(evlog, "Time step = " << dt);
        
        for( unsigned iComponent = 0; iComponent < tsData.GetNComponents(); ++iComponent )
        {
            KTDEBUG(evlog, "Processing component: " << iComponent);

            // get TS from data object and make the new TS
            ts = dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries( iComponent ));
            
            // Loop through all bins
            for( unsigned iBin = 0; iBin < ts->GetNTimeBins(); ++iBin )
            {
                // Obtain norm and phase from components
                norm = sqrt( (*ts)(iBin)[0] * (*ts)(iBin)[0] + (*ts)(iBin)[1] * (*ts)(iBin)[1] );
                phase = atan2( (*ts)(iBin)[1], (*ts)(iBin)[0] );

                // Shift phase
                phase -= tCharge * tMagneticFieldEffective / (tMass * tGamma * tAxialFrequency) * tBetaZ0 * sin(tAxialFrequency * t + tAxialPhase);

                // Assign components from norm and new phase
                (*ts)(iBin)[0] = norm * cos( phase );
                (*ts)(iBin)[1] = norm * sin( phase );

                // Increment time value
                t += dt;
            }
        }

        return true;
    }

    void KTWarblet::SlotFunctionTS( Nymph::KTDataPtr data )
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
        if(! DewarbleTS( newData->Of< KTTimeSeriesData >(), newData->Of< KTSliceHeader >() ))
        {
            KTERROR(avlog_hh, "Something went wrong while analyzing time series data!");
            return;
        }

        // Emit signal
        fTSSignal( newData );
    
        return;
    }

    void KTWarblet::SlotFunctionHeader( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTEggHeader >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTEggHeader >!");
            return;
        }
    }

} // namespace Katydid
