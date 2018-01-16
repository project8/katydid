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
#include "KTTimeSeriesReal.hh"
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

        SetPitchAngle(node->get_value< double >("pitch", fPitchAngle) * KTMath::Pi() / 180. );
        SetAxialPhase(node->get_value< double >("axial_phase", fAxialPhase) * KTMath::Pi() / 180.); //Convert to radians immediately
        SetMethod(node->get_value< std::string >("method", fMethod));

        return true;
    }

    bool KTWarblet::DewarbleTS( KTTimeSeriesData& tsData, KTSliceHeader& slice )
    {
        KTDEBUG(evlog, "Receiving time series for dewarbling");

        // Slice and TS parameters

        double t = slice.GetTimeInAcq();    // time value of the current bin
        double dt = slice.GetBinWidth();    // time step

        const double tCharge = 1.602176e-19;
        const double tMass = 9.10938356e-31;
        const double tMassEV = 510.9989461e3;
        const double tC = 2.99792458e8;
        const double tEnergy = 17830.0; //Expected energy .... we will need to think about this for H3
        const double tGamma = 1. + tEnergy / tMassEV;
        const double tBeta = sqrt(1. - 1. / (tGamma * tGamma));
        const double tMuNull = 4. * KTMath::Pi() * 1e-7;

        //////////// Trap Parameters (Hardcoding is 4 dummies!) ///////////
        const double B0 = 0.9583;
        const double L0 = 0.355;
        const double L1 = 0.042;

        double tPitchAngle = GetPitchAngle();
        double tAxialPhase = GetAxialPhase();

        double tCyclotronFrequency = tCharge * B0 / ( tMass * tGamma);

        double tBetaZ0 = tBeta * cos(tPitchAngle);
        double tZMax = L0 * cos(tPitchAngle) / sin(tPitchAngle);
        double tAxialFrequency = tBetaZ0 * tC / tZMax;

        double tPeriod = 2. * L1 / (tBetaZ0 * tC) + 2. * KTMath::Pi() / tAxialFrequency;

        //Times that define the piecewise function for a particle in a bathtub trap (ie turning point/ flat region, etc.)
        double tTimeRange[4];

        tTimeRange[0] = L1/(tBetaZ0 * tC);
        tTimeRange[1] = L1/(tBetaZ0 * tC) + KTMath::Pi() / tAxialFrequency;
        tTimeRange[2] = 2. * L1/(tBetaZ0 * tC) + KTMath::Pi() / tAxialFrequency;
        tTimeRange[3] = 2. * L1/(tBetaZ0 * tC) + 2. * KTMath::Pi() / tAxialFrequency;
        
        t+= tAxialPhase / (2. * KTMath::Pi() ) * tTimeRange[3];

        KTINFO(evlog, "Set up time series and slice parameters.");
        KTDEBUG(evlog, "Initial t = " << t);
        KTDEBUG(evlog, "Time step = " << dt);
        KTDEBUG(evlog, "tPitchAngle = " << tPitchAngle);
        KTDEBUG(evlog, "tAxialPhase = " << tAxialPhase);
        KTDEBUG(evlog, "tAxialFrequency = " << tAxialFrequency);
        KTDEBUG(evlog, "tRange = " << tTimeRange[0]<<" "<<tTimeRange[1]<<" "<<tTimeRange[2]<<" "<<tTimeRange[3]);

        double phase = 0.;                  // argument of current TS value
        double tOldSignal[2];

        KTTimeSeriesFFTW* ts;   // time series from data
        double tEffective;
        double tCycles;
    
        for( unsigned iComponent = 0; iComponent < tsData.GetNComponents(); ++iComponent )
        {
            KTDEBUG(evlog, "Processing component: " << iComponent);

            // get TS from data object and make the new TS
            ts = dynamic_cast< KTTimeSeriesFFTW* >(tsData.GetTimeSeries( iComponent ));
            //KTTimeSeriesReal* ts = dynamic_cast< KTTimeSeriesReal* >(tsData.GetTimeSeries(iComponent));
            
            // Loop through all bins
            for( unsigned iBin = 0; iBin < ts->GetNTimeBins(); ++iBin )
            {
                tCycles=int(t/tTimeRange[3]);
                tEffective = t - tCycles*tTimeRange[3] ;
                // Shift phase
                if(tEffective >= 0 && tEffective<=tTimeRange[0]) 
                    phase = tCyclotronFrequency * tBetaZ0 * tEffective;
                else if(tEffective > tTimeRange[0] && tEffective<=tTimeRange[1]) 
                    phase = tCyclotronFrequency * ( L1 / tC + pow(cos(tPitchAngle) / sin(tPitchAngle) , 2.) / 2. *(tEffective-tTimeRange[0]) + tBetaZ0 / tAxialFrequency * sin(tAxialFrequency*(tEffective-tTimeRange[0])));
                else if(tEffective > tTimeRange[1] && tEffective<=tTimeRange[2]) 
                    phase = tCyclotronFrequency * ( L1 / tC + pow(cos(tPitchAngle) / sin(tPitchAngle) , 2.) / 2. * KTMath::Pi() / tAxialFrequency - tBetaZ0 * (tEffective - tTimeRange[1]));
                else if(tEffective > tTimeRange[2] && tEffective<=tTimeRange[3]) 
                    phase = tCyclotronFrequency * ( pow(cos(tPitchAngle) / sin(tPitchAngle) , 2.) / 2. *(tEffective-tTimeRange[2] + KTMath::Pi() / tAxialFrequency) - tBetaZ0 / tAxialFrequency * sin(tAxialFrequency*(tEffective-tTimeRange[2])));
                else
                {
                    KTERROR(avlog_hh, "Time not in range of bathtub model!!!");
                    KTERROR(avlog_hh, "Time "<<tEffective<<" "<<tTimeRange[3]);
                }

                phase += tCycles*pow(cos(tPitchAngle)/sin(tPitchAngle),2.) * KTMath::Pi() * tCyclotronFrequency/tAxialFrequency;

                tOldSignal[0] = (*ts)(iBin)[0];
                tOldSignal[1] = (*ts)(iBin)[1];
                // Assign components from norm and new phase
                (*ts)(iBin)[0] = tOldSignal[0] * cos(phase) + tOldSignal[1] * sin(phase);
                (*ts)(iBin)[1] = -tOldSignal[0] * sin(phase) + tOldSignal[1] * cos(phase);

                //(*ts)(iBin) *= cos(phase);

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
