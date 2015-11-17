/*
 * KTLinearDensityProbeFit.cc
 *
 *  Created on: Nov 13, 2015
 *      Author: ezayas
 */

#include "KTLinearDensityProbeFit.hh"

#include "KTParam.hh"

#include <cmath>
#include <vector>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::string;
using std::vector;


namespace Katydid
{
    KTLOGGER(sdlog, "KTLinearDensityProbeFit");

    KT_REGISTER_PROCESSOR(KTLinearDensityProbeFit, "linear-density-fit");

    KTLinearDensityProbeFit::KTLinearDensityProbeFit(const std::string& name) :
            KTProcessor(name),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fMinBin(0),
            fMaxMin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fProbeWidth(1e6),
            fStepSize(0.2e6),
            fLinearDensityFitSignal("linear-density-fit", this),
            fThreshPointsSlot("thresh-points", this, &KTLinearDensityProbeFit::Calculate, &fLinearDensityFitSignal),
    {
    }

    KTLinearDensityProbeFit::~KTLinearDensityProbeFit()
    {
    }

    bool KTLinearDensityProbeFit::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        if (node->Has("min-frequency"))
        {
            SetMinFrequency(node->GetValue< double >("min-frequency"));
        }
        if (node->Has("max-frequency"))
        {
            SetMaxFrequency(node->GetValue< double >("max-frequency"));
        }
        if (node->Has("min-bin"))
        {
            SetMinBin(node->GetValue< unsigned >("min-bin"));
        }
        if (node->Has("max-bin"))
        {
            SetMaxBin(node->GetValue< unsigned >("max-bin"));
        }
        if (node->Has("probe-width"))
        {
            SetProbeWidth(node->GetValue< double >("probe-width"));
        }
        if (node->Has("step-size"))
        {
            SetStepSize(node->GetValue< double >("step-size"));
        }

        return true;
    }

    double Gaus_Eval( double arg, double sigma )
    {
        return exp( pow(arg/sigma, 2)/2 );
    }

    bool KTLinearDensityProbeFit::Calculate(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts)
    {
        KTLinearFitResult& newData = data.Of< KTLinearFitResult >();

        if (fCalculateMinBin)
        {
            SetMinBin(data.GetSpectra()[0]->FindBin(fMinFrequency));
            KTDEBUG(sdlog, "Minimum bin set to " << fMinBin);
        }
        if (fCalculateMaxBin)
        {
            SetMaxBin(data.GetSpectra()[0]->FindBin(fMaxFrequency));
            KTDEBUG(sdlog, "Maximum bin set to " << fMaxBin);
        }

        double alpha = fMinFrequency;
        double bestAlpha = 0, bestError = 0, error = 0;
        while( alpha <= fMaxFrequency )
        {
            error = 0;

            // Calculate the associated error to the current value of alpha
            for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = pts.GetSetOfPoints(0).begin(); it != pts.GetSetOfPoints(0).end(); ++it )
            {
                error -= Gaus_Eval( it->second.fOrdinate - data.fSlope * it->second.fAbscissa - alpha, fProbeWidth );
            }

            if( error < bestError || bestError == 0 )
            {
                bestError = error;
                bestAlpha = alpha;
            }
            
            // Increment alpha
            alpha += fStepSize;
        }

        newData.SetIntercept( bestAlpha, 0 );
    }


} /* namespace Katydid */