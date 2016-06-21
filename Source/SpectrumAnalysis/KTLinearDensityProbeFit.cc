/*
 * KTLinearDensityProbeFit.cc
 *
 *  Created on: Nov 13, 2015
 *      Author: ezayas
 */

#include "KTLinearDensityProbeFit.hh"
#include "KTDiscriminatedPoints2DData.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTProcessedTrackData.hh"
#include "KTLinearFitResult.hh"
#include "KTSpectrumCollectionData.hh"
#include "KTLogger.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesReal.hh"
#include "KTTimeSeriesData.hh"
#include "KTEggHeader.hh"

#include "KTParam.hh"

#include <cmath>
#include <vector>
#include <algorithm>

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
            fMaxBin(1),
            fCalculateMinBin(true),
            fCalculateMaxBin(true),
            fProbeWidthBig(1e6),
            fProbeWidthSmall(0.02e6),
            fStepSizeBig(0.2e6),
            fStepSizeSmall(0.004e6),
            fLinearDensityFitSignal("fit-result", this),
            fTimeSeriesSignal("ts", this),
            fTSHeaderSignal("ts-header", this),
            fThreshPointsSlot("thresh-points", this, &KTLinearDensityProbeFit::Calculate, &fLinearDensityFitSignal),
            fPreCalcSlot("gv", this, &KTLinearDensityProbeFit::SetPreCalcGainVar)
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
        return exp( -1*pow(arg/sigma, 2)/2 );
    }

    double Significance( vector<double> x, vector<int> omit, int include, std::string metric )
    {
        double noiseAmp = 0;
        double noiseDev = 0;

        int s = x.size();
        for( int i = 0; i < s; i++ )
        {
            if( find( omit.begin(), omit.end(), i ) == omit.end() )
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

    double KTLinearDensityProbeFit::findIntercept( KTDiscriminatedPoints2DData& pts, double dalpha, double q, double width )
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

    bool KTLinearDensityProbeFit::SetPreCalcGainVar(KTGainVariationData& gvData)
    {
        fGVData = gvData;
        return true;
    }

    bool KTLinearDensityProbeFit::Calculate(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts)
    {
        KTLinearFitResult& newData = data.Of< KTLinearFitResult >();
        KTPSCollectionData& fullSpectrogram = data.Of< KTPSCollectionData >();
/*
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
*/
        newData.SetNComponents( 2 );
        newData.SetSlope( data.GetSlope(), 0 );
        newData.SetSlope( data.GetSlope(), 1 );
        newData.SetSlopeSigma( data.GetSlopeSigma(), 0 );
        newData.SetSlopeSigma( data.GetSlopeSigma(), 1 );
        newData.SetTrackDuration( data.GetEndTimeInRunC() - data.GetStartTimeInRunC(), 0 );
        newData.SetTrackDuration( data.GetEndTimeInRunC() - data.GetStartTimeInRunC(), 1 );

        double intercept1, intercept2;

        PerformTest( pts, newData, fProbeWidthBig, fStepSizeBig, 0 );
        PerformTest( pts, newData, fProbeWidthSmall, fStepSizeSmall, 1 );

        KTDEBUG(sdlog, "Beginning fourier analysis of sideband" );

        intercept1 = newData.GetIntercept( 0 );
        intercept2 = newData.GetIntercept( 1 );

        newData.SetSidebandSeparation( intercept1 - intercept2, 0 );
        newData.SetSidebandSeparation( intercept1 - intercept2, 1 );

        // We will need to calculate the unweighted projection first
        double delta_f;
        double alphaBound_upper = intercept1 + 1e6;
        double alphaBound_lower = intercept1 - 1e6;

        int xBinStart, xBinEnd, xWindow, yBinStart, yBinEnd, yWindow;
        double ps_xmin, ps_xmax, ps_ymin, ps_dx, ps_dy;
        double q_fit = newData.GetSlope( 0 );
        double x, y;
        
        ps_xmin = fullSpectrogram.GetStartTime();
        ps_xmax = fullSpectrogram.GetEndTime();
        ps_ymin = fullSpectrogram.GetSpectra().begin()->second->GetRangeMin();
        ps_dx   = fullSpectrogram.GetDeltaT();
        ps_dy   = fullSpectrogram.GetSpectra().begin()->second->GetFrequencyBinWidth();
        
        // We add +1 for the underflow bin
        xBinStart = floor( (data.GetStartTimeInRunC() - ps_xmin) / ps_dx ) + 1;
        xBinEnd   = floor( (data.GetEndTimeInRunC() - ps_xmin) / ps_dx ) + 1;
        xWindow = xBinEnd - xBinStart + 1;
        KTINFO(sdlog, "Set xBin range to " << xBinStart << ", " << xBinEnd);

        newData.SetFit_width( xWindow, 0 );
        newData.SetFit_width( xWindow, 1 );

        // The y window this time will be floating, but its size will be consistent
        // The number of bins between the alpha bounds
        yWindow = ceil( (alphaBound_upper - alphaBound_lower) / ps_dy );

        //double *unweighted = new double[xWindow];
        vector< double > unweighted, weighted, fourier;

        KTDEBUG(sdlog, "Computing unweighted projection");

        int i = 0;
        // First we compute the unweighted projection
        for( std::map< double, KTPowerSpectrum* >::const_iterator it = fullSpectrogram.GetSpectra().begin(); it != fullSpectrogram.GetSpectra().end(); ++it )
        {
            // Set x value and starting y-bin
            x = ps_xmin + (i - 1) * ps_dx;
            yBinStart = it->second->FindBin( alphaBound_lower + q_fit * x );

            // Unweighted power = sum of raw power spectrum
            unweighted.push_back( 0 );
            for( int j = yBinStart; j < yBinStart + yWindow; j++ )
            {
                y = ps_ymin + ps_dy * (j - 1);

                // We reevaluate the spline rather than deal with the appropriate index of power_minus_bkgd
                unweighted[i] += (*it->second)(j) - fGVData.GetSpline()->Evaluate( y );
            }
            i++;
        }

        KTDEBUG(sdlog, "Computing weighted projection");

        i = 0;

        // Weighted projection
        double cumulative;
        for( std::map< double, KTPowerSpectrum* >::const_iterator it = fullSpectrogram.GetSpectra().begin(); it != fullSpectrogram.GetSpectra().end(); ++it )
        {
            cumulative = 0.;

            x = ps_xmin + (i - 1) * ps_dx;
            yBinStart = it->second->FindBin( alphaBound_lower + q_fit * x );

            for( int j = yBinStart; j < yBinStart + yWindow; j++ )
            {
                y = ps_ymin + ps_dy * (j - 1);

                // Calculate delta-f using the fit values
                delta_f = y - (q_fit * x + newData.GetIntercept(0));
                cumulative += delta_f * ((*it->second)(j) - fGVData.GetSpline()->Evaluate( y )) / unweighted[i];
            }

            weighted.push_back( cumulative );
            i++;
        }

        double pi = 3.14159265358979;
        for( int k = 0; k < xWindow; k++ )
        {
            cumulative = 0.;
            for( int n = 1; n <= xWindow - 2; n++ )
                cumulative += weighted[n] * cos( n * k * pi / (xWindow - 1) );

            fourier.push_back( pow( 0.5 * (weighted[0] + pow( -1, k ) * weighted[xWindow - 1]) + cumulative, 2 ) );
        }

        double avg_fourier = 0.;
        double max_fourier = 0.;
        double freq_step = 1/(2 * (xWindow - 1) * ps_dx);

        for( int k = 0; k < xWindow; k++ )
            avg_fourier += fourier[k] / xWindow;

        for( int k = 0; k < xWindow; k++ )
        {
            if( fourier[k] > max_fourier )
            {
                max_fourier = fourier[k];
                newData.SetFFT_peak( double(k) * freq_step, 0 );
                newData.SetFFT_SNR( max_fourier / avg_fourier, 0 );
                newData.SetFFT_peak( double(k) * freq_step, 1 );
                newData.SetFFT_SNR( max_fourier / avg_fourier, 1 );
            }
        }

        KTINFO(sdlog, "Successfully obtained power modulation. Maximum fourier peak at frequency " << newData.GetFFT_peak( 0 ) << " with SNR " << newData.GetFFT_SNR( 0 ));

        return true;
    }

    bool KTLinearDensityProbeFit::PerformTest(KTDiscriminatedPoints2DData& pts, KTLinearFitResult& newData, double fProbeWidth, double fStepSize, unsigned component)
    {
        double alpha = fMinFrequency;
        double bestAlpha = 0, bestError = 0, error = 0;
        
        KTINFO(sdlog, "Slope = " << newData.GetSlope( component ));
        bestAlpha = findIntercept( pts, fStepSize, newData.GetSlope( component ), fProbeWidth );
        newData.SetIntercept( bestAlpha, component );



/*
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
        double alphaBound_lower;
        double alphaBound_upper;

        if( component == 1 )    // Signal
        {
            alphaBound_lower = bestAlpha - 10 * fProbeWidth;
            alphaBound_upper = bestAlpha + 10 * fProbeWidth;
        }
        else                    // Sideband
        {
            alphaBound_lower = bestAlpha - 10 * fProbeWidth;
            alphaBound_upper = bestAlpha + 10 * fProbeWidth;
            /*
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

            KTDEBUG(sdlog, "Coarse sweep best fit intercept = " << bestAlpha);
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

            vector<int> candidates;
            candidates.push_back( bestLocalMin );
            candidates.push_back( nextBestLocalMin );

            newData.SetFineProbe_sigma_1( Significance( localMinValues, candidates, bestLocalMin, "Sigma" ), component );
            newData.SetFineProbe_sigma_2( Significance( localMinValues, candidates, nextBestLocalMin, "Sigma" ), component );
            newData.SetFineProbe_SNR_1( Significance( localMinValues, candidates, bestLocalMin, "SNR" ), component );
            newData.SetFineProbe_SNR_2( Significance( localMinValues, candidates, nextBestLocalMin, "SNR" ), component );

            alphaBound_lower = localMins[std::min( bestLocalMin, nextBestLocalMin )];
            alphaBound_upper = localMins[std::max( bestLocalMin, nextBestLocalMin )];

            // We will push the lower bound down from the left-most of the two minima
            // until its error exceeds the threshold
            double threshold = 0;
            for( int i = std::min( bestLocalMin, nextBestLocalMin ) + 1; i < std::max( bestLocalMin, nextBestLocalMin ); i++ )
                if( localMinValues[i] < threshold || threshold == 0 )
                    threshold = localMinValues[i];
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




        }

        KTDEBUG(sdlog, "Fine sweep intercept bounds: " << alphaBound_lower << " - " << alphaBound_upper);

        // Final cut
        // Keep only points for which y - q*x falls between the alpha bounds

        vector<double> finalCutX, finalCutY;                                                                        
        int nFinal = 0;

        // Loop through the outliers
        for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = pts.GetSetOfPoints(0).begin(); it != pts.GetSetOfPoints(0).end(); ++it )
        {
            alpha = it->second.fOrdinate - newData.GetSlope( component ) * it->second.fAbscissa;
            if( alpha > alphaBound_lower && alpha < alphaBound_upper )
            {
                finalCutX.push_back( it->second.fAbscissa );
                finalCutY.push_back( it->second.fOrdinate );
                nFinal++;
            }
        }
        KTDEBUG(sdlog, "Performed final cut. Passed " << nFinal << " points");

        newData.SetFit_width( alphaBound_upper - alphaBound_lower, component );
        newData.SetNPoints( nFinal, component );

        // Calculate best-fit slope and intercept
        double eXY = 0, eX = 0, eY = 0, eX2 = 0;
        int s = finalCutX.size();

        for( int i = 0; i < s; i++ )
        {
            eXY += finalCutX[i] * finalCutY[i] / s;
            eX += finalCutX[i] / s;
            eY += finalCutY[i] / s;
            eX2 += pow( finalCutX[i], 2 ) / s;
        }

        //newData.SetSlope( (eXY - eX * eY)/(eX2 - pow( eX, 2 )), component );
        newData.SetIntercept( eY - newData.GetSlope( component ) * eX, component );
        newData.SetProbeWidth( fProbeWidth, component );
        //newData.SetStartingFrequency( eY - newData.GetSlope( component ) * eX + newData.GetSlope( component ) * newData.GetStartTimeInRunC( component ), component );
*/
        return true;
    }


} /* namespace Katydid */