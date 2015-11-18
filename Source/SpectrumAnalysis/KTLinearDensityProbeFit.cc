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
            fProbeWidthBig(1e6),
            fProbeWidthSmall(0.02e6),
            fStepSizeBig(0.2e6),
            fStepSizeSmall(0.004e6),
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
        if (node->Has("probe-width-big"))
        {
            SetProbeWidthBig(node->GetValue< double >("probe-width-big"));
            SetStepSizeBig(node->GetValue< double >("probe-width-big") / 5);
        }
        if (node->Has("probe-width-small"))
        {
            SetProbeWidthSmall(node->GetValue< double >("probe-width-small"));
            SetStepSizeSmall(node->GetValue< double >("probe-width-small") / 5);
        }
        if (node->Has("step-size-big"))
        {
            SetStepSizeBig(node->GetValue< double >("step-size-big"));
        }
        if (node->Has("step-size-small"))
        {
            SetStepSizeSmall(node->GetValue< double >("step-size-small"));
        }

        return true;
    }

    double Gaus_Eval( double arg, double sigma )
    {
        return exp( pow(arg/sigma, 2)/2 );
    }

    double Significance( vector<double> x, vector<uint64_t> omit, uint64_t include, char* metric )
    {
        double noiseAmp = 0;
        double noiseDev = 0;

        uint64_t s = x.size();
        for( uint64_t i = 0; i < s; i++ )
        {
            if( find( omit.begin(), omit.end() ) == omit.end() )
            {
                noiseAmp += x[i] / s;
                noiseDev += pow( x[i], 2 ) / s;
            }
        }
        noiseDev = sqrt( noiseDev - pow( noiseAmp, 2 ) );

        if( metric == "Sigma" )
            return (x[include] - noiseAmp) / noiseDev;
        else if( metric == "SNR" )
            return x[include] / noiseAmp;
        else
            return -1.;
    }

    double findIntercept( KTDiscriminatedPoints2DData& pts, double dalpha, double q, double width )
    {
        double alpha = fMinFrequency;
        double bestAlpha = 0, bestError = 0, error = 0;
        while( alpha <= fMaxFrequency )
        {
            error = 0;

            // Calculate the associated error to the current value of alpha
            for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = pts.GetSetOfPoints(0).begin(); it != pts.GetSetOfPoints(0).end(); ++it )
            {
                error -= Gaus_Eval( it->second.fOrdinate - q * it->second.fAbscissa - alpha, width );
            }

            if( error < bestError || bestError == 0 )
            {
                bestError = error;
                bestAlpha = alpha;
            }
            
            // Increment alpha
            alpha += dalpha;
        }

        return bestAlpha;
    }

    bool KTLinearDensityProbeFit::Calculate(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts)
    {
        KTLinearFitResult& newData = data.Of< KTLinearFitResult >();
        newData.SetSlope( data.fSlope );

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

        newData.SetNComponents(2);
        PerformTest( pts, newData, fProbeWidthBig, fStepSizeBig, 0 );
        PerformTest( pts, newData, fProbeWidthSmall, fStepSizeSmall, 1 );

        return true;
    }

    bool KTLinearDensityProbeFit::PerformTest(KTDiscriminatedPoints2DData& pts, KTLinearFitResult& newData, double fProbeWidth, double fStepSize, unsigned component)
    {
        double alpha = fMinFrequency;
        double bestAlpha = 0, bestError = 0, error = 0;
        
        bestAlpha = findIntercept( pts, fStepSize, newData.GetSlope( component ), fProbeWidth );
        newData.SetIntercept( bestAlpha, component );

        double alphaMean = 0.;
        double alphaVar = 0.;
        double t = 0.;

        for( double squeeze = 0.55; squeeze <= 1.45; squeeze += 0.1 )
        {
            t = findIntercept( pts, fStepSize, newData.GetSlope( component ), fProbeWidth * squeeze );
            alphaMean += t / 10;
            alphaVar += pow( t, 2 ) / 10;
        }
        alphaVar = sqrt( alphaVar - pow( alphaMean, 2 ) );
        newData.SetIntercept_deviation( alphaVar, component );

        // Next we begin the fine sweep of alpha
        alpha = bestAlpha - 2 * fProbeWidth;
        double smallStep = 1e3, smallWidth = 1e4;

        // We will be interested in local minima for setting a threshold of points to keep
        // These vectors will hold the locations and values of all local minima
        // found during the fine sweep

        vector<double> localMins;
        vector<double> localMinValues;

        // We expect two largely pronounced minima from the sideband boundaries
        uint64_t bestLocalMin = -1, nextBestLocalMin = -1;
        uint64_t nLocalMins = 0;

        // Temp variables used for identifying local minima
        double error_x1 = 0, error_x2 = 0;

        while( alpha <= bestAlpha + 2 * fProbeWidth )
        {
            // Calculate the associated error
            error = 0;
            for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = pts.GetSetOfPoints(0).begin(); it != pts.GetSetOfPoints(0).end(); ++it )
            {
                error -= Gaus_Eval( it->second.fOrdinate - newData.GetSlope( component ) * it->second.fAbscissa - alpha, smallWidth );
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

        vector<uint64_t> candidates;
        candidates.push_back( bestLocalMin );
        candidates.push_back( nextBestLocalMin );

        newData.SetFineProbe_sigma_1( Significance( localMinValues, candidates, bestLocalMin, "Sigma" ), component );
        newData.SetFineProbe_sigma_2( Significance( localMinValues, candidates, nextBestLocalMin, "Sigma" ), component );
        newData.SetFineProbe_SNR_1( Significance( localMinValues, candidates, bestLocalMin, "SNR" ), component );
        newData.SetFineProbe_SNR_2( Significance( localMinValues, candidates, nextBestLocalMin, "SNR" ), component );

        double alphaBound_lower = localMins[min( bestLocalMin, nextBestLocalMin )];
        double alphaBound_upper = localMins[max( bestLocalMin, nextBestLocalMin )];

        // We will push the lower bound down from the left-most of the two minima
        // until its error exceeds the threshold
        error = 0;
        while( error < threshold )
        {
            alphaBound_lower -= smallStep;
            error = 0;
            for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = pts.GetSetOfPoints(0).begin(); it != pts.GetSetOfPoints(0).end(); ++it )
            {
                error -= Gaus_Eval( it->second.fOrdinate - newData.GetSlope(component) * it->second.fAbscissa - alphaBound_lower, smallWidth );
            }
        }

        // And the same for the upper bound
        error = 0;
        while( error < threshold )
        {
            alphaBound_upper += smallStep;
            error = 0;
            for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = pts.GetSetOfPoints(0).begin(); it != pts.GetSetOfPoints(0).end(); ++it )
            {
                error -= Gaus_Eval( it->second.fOrdinate - newData.GetSlope( component ) * it->second.fAbscissa - alphaBound_upper, smallWidth );
            }
        }

        // Final cut
        // Keep only points for which y - q*x falls between the alpha bounds

        vector<double> finalCutX, finalCutY;                                                                        
        uint64_t nFinal = 0;

        // Loop through the outliers
        for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = pts.GetSetOfPoints(0).begin(); it != pts.GetSetOfPoints(0).end(); ++it )
        {
            alpha = it->second.fOrdinate - data.fSlope * it->second.fAbscissa;
            if( alpha > alphaBound_lower && alpha < alphaBound_upper )
            {
                finalCutX.push_back( it->second.fAbscissa );
                finalCutY.push_back( it->second.fOrdinate );
                nFinal++;
            }
        }

        newData.SetFitWidth( alphaBound_upper - alphaBound_lower, component );
        newData.SetNPoints( nFinal, component );

        double eXY = 0, eX = 0, eY = 0, eX2 = 0;
        uint64_t s = finalCutX.size();

        for( uint64_t i = 0; i < s; i++ )
        {
            eXY += finalCutX[i] * finalCutY[i] / s;
            eX += finalCutX[i] / s;
            eY += finalCutY[i] / s;
            eX2 += pow( finalCutX[i], 2 ) / s;
        }

        newData.SetSlope( (eXY - eX * eY)/(eX2 - pow( eX, 2 )), component );
        newData.SetIntercept( eY - newData.GetSlope( component ) * eX, component );
        newData.SetProbeWidth( fProbeWidth, component );

        return true;
    }


} /* namespace Katydid */