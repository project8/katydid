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
            SetStepSize(node->GetValue< double >("probe-width") / 5);
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

    double Significance( vector<double> x, vector<int> omit, char* metric )
    {
        double noiseAmp = 0, signalAmp = 0;
        double noiseDev = 0, signalDev = 0;

        for( int i = 0; i < x.size(); i++ )
        {
            if( find( omit.begin(), omit.end() ) == omit.end() )
            {
                noiseAmp += x[i];
                noiseDev += pow( x[i], 2 );
            }
            else
            {
                signalAmp += x[i];
                signalDev += pow( x[i], 2 );
            }
        }
        noiseDev = sqrt( noiseDev - pow( noiseAmp, 2 ) );
        signalDev = sqrt( signalDev - pow( signalAmp, 2 ) );

        if( metric == "Sigma" )
            return (signalAmp - noiseAmp) / sqrt( pow( signalDev, 2 ) + pow( noiseDev, 2 ) );
        else if( metric == "SNR" )
            return signalAmp / noiseAmp;
        else
            return -1.;
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

        // Next we begin the fine sweep of alpha
        alpha = bestAlpha - 2 * fProbeWidth;
        double smallStep = 1e3, smallWidth = 1e4;

        // We will be interested in local minima for setting a threshold of points to keep
        // These vectors will hold the locations and values of all local minima
        // found during the fine sweep

        vector<double> localMins;
        vector<double> localMinValues;

        // We expect two largely pronounced minima from the sideband boundaries
        int bestLocalMin = -1, nextBestLocalMin = -1;
        int nLocalMins = 0;

        // Temp variables used for identifying local minima
        double error_x1 = 0, error_x2 = 0;

        while( alpha <= bestAlpha + 2 * fProbeWidth )
        {
            // Calculate the associated error
            error = 0;
            for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = pts.GetSetOfPoints(0).begin(); it != pts.GetSetOfPoints(0).end(); ++it )
            {
                error -= Gaus_Eval( it->second.fOrdinate - data.fSlope * it->second.fAbscissa - alpha, smallWidth );
            }

            // A local minima is defined simply as a point x_n where
            // x_n < x_n-1 and x_n < x_n+1
            if( error > error_x1 && error_x2 > error_x1 && error_x1 != 0 && error_x2 != 0 )
            {
                // Local minimum found for the previous point
                localMins.push_back( alpha - smallStep );
                localMinValues.push_back( error_x1 );

                // If the local minimum is the best so far
                if( bestLocalMin == -1 || error < localMinValues[bestLocalMin] )
                {
                    nextBestLocalMin = bestLocalMin;

                    // nLocalMins has not yet been incremented
                    // This accurately corresponds to the vector index
                    bestLocalMin = nLocalMins;
                }
                else if( nextBestLocalMin == -1 || error < localMinValues[nextBestLocalMin] )
                    nextBestLocalMin = nLocalMins;

                // Increment nLocalMins
                nLocalMins++;
            }

            // Cleanup
            alpha += smallStep;
            error_x2 = error_x1;
            error_x1 = error;

        } // End fine sweep of alpha

        vector<int> candidates;
        candidates.push_back( bestLocalMin );
        candidates.push_back( nextBestLocalMin );

        newData.SetFineProbe_sigma( Significance( localMinValues, candidates, "Sigma" ), 0 );
        newData.SetFineProbe_SNR( Significance( localMinValues, candidates, "SNR" ), 0 );
    }


} /* namespace Katydid */