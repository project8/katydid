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
#include "KTPowerFitData.hh"
#include "KTSpectrumCollectionData.hh"
#include "KTLogger.hh"
#include "KTTimeSeries.hh"
#include "KTTimeSeriesReal.hh"
#include "KTTimeSeriesData.hh"
#include "KTEggHeader.hh"

#include <cmath>
#include <vector>
#include <algorithm>

#ifdef USE_OPENMP
#include <omp.h>
#endif

#ifdef ROOT_FOUND
#include "TH1.h"
#include "TF1.h"
#include "KT2ROOT.hh"
#include "TMath.h"
#include "TFitResult.h"
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
            fProbeWidthBig(1e6),
            fProbeWidthSmall(0.02e6),
            fStepSizeBig(0.2e6),
            fStepSizeSmall(0.004e6),
            fLinearDensityFitSignal("fit-result", this),
            fPowerFitSignal("power-fit-result", this),
            fThreshPointsSlot("thresh-points", this, &KTLinearDensityProbeFit::Calculate, &fLinearDensityFitSignal),
            fThreshPointsAltSlot("thresh-points-alt", this, &KTLinearDensityProbeFit::CalculatePower, &fPowerFitSignal),
            fPreCalcSlot("gv", this, &KTLinearDensityProbeFit::SetPreCalcGainVar)
    {
    }

    KTLinearDensityProbeFit::~KTLinearDensityProbeFit()
    {
    }

    bool KTLinearDensityProbeFit::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        if (node->has("min-frequency"))
        {
            SetMinFrequency(node->get_value< double >("min-frequency"));
        }
        if (node->has("max-frequency"))
        {
            SetMaxFrequency(node->get_value< double >("max-frequency"));
        }
        if (node->has("probe-width-big"))
        {
            SetProbeWidthBig(node->get_value< double >("probe-width-big"));
            SetStepSizeBig(node->get_value< double >("probe-width-big") / 5);
        }
        if (node->has("probe-width-small"))
        {
            SetProbeWidthSmall(node->get_value< double >("probe-width-small"));
            SetStepSizeSmall(node->get_value< double >("probe-width-small") / 5);
        }

        SetStepSizeBig(node->get_value< double >("step-size-big", fStepSizeBig));
        SetStepSizeSmall(node->get_value< double >("step-size-small", fStepSizeSmall));

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

    Double_t fitf(Double_t *x, Double_t *par)
    {

    /*
        The inputs to this method must be x and par
        This is the only way root can use the method for fitting

        x is an array containing the arguments of the function
        In this case, the function is 1D so I will only use x[0]

        par is an array for the parameters in the fit:
        par[0] = Overall scale
        par[1] = Overall background
        par[2] = Overall frequency center in MHz
        par[3] = Curvature
        par[4] = Width in MHz
    */

        // Control constants
        double np = 1000.0;        // number of convolution steps
        double sc =   5.0;         // convolution extends to +-sc Gaussian sigmas

        // Variables
        double xx;
        double fSec;
        double sum = 0.0;
        double xlow,xupp;
        double step;
        double i;

        // Range of convolution integral
        xlow = -0.5*par[4];
        xupp = 0.5*par[4];

        step = (xupp-xlow) / np;

        // Convolution integral of Landau and Gaussian by sum
        for(i=1.0; i<=np/2; i++) {
          xx = xlow + (i-.5) * step;
          fSec = TMath::Power( TMath::Cos( par[3] * xx / (par[4]) ), -2 );
          sum += fSec * TMath::Gaus(x[0]-par[2]*1e6,xx,100000);

          xx = xupp - (i-.5) * step;
          fSec = TMath::Power( TMath::Cos( par[3] * xx / (par[4]) ), -2 );
          sum += fSec * TMath::Gaus(x[0]-par[2]*1e6,xx,100000);
        }

        return par[0] * sum + par[1];
    }

    bool KTLinearDensityProbeFit::Calculate(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts)
    {
        KTLinearFitResult& newData = data.Of< KTLinearFitResult >();
        KTPSCollectionData& fullSpectrogram = data.Of< KTPSCollectionData >();

        newData.SetNComponents( 2 );
        newData.SetSlope( data.GetSlope(), 0 );
        newData.SetSlope( data.GetSlope(), 1 );
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

        // Discrete Cosine Transform (real -> real) of type I
        // Explicit, not fast (i.e. n^2 operations)

        double pi = 3.14159265358979;
        for( int k = 0; k < xWindow; k++ )
        {
            cumulative = 0.;
            for( int n = 1; n <= xWindow - 2; n++ )
                cumulative += weighted[n] * cos( n * k * pi / (xWindow - 1) );

            fourier.push_back( pow( 0.5 * (weighted[0] + pow( -1, k ) * weighted[xWindow - 1]) + cumulative, 2 ) );
        }

        // Evaluate SNR and std dev significance of the largest fourier peak

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

    bool KTLinearDensityProbeFit::CalculatePower(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts)
    {
        KTPowerFitData& newData = data.Of< KTPowerFitData >();
        KTPSCollectionData& fullSpectrogram = data.Of< KTPSCollectionData >();

        newData.SetNComponents( 1 );

        double alpha = data.GetStartFrequency() - 5e6;
        double error;
        double q = data.GetSlope();

        while( alpha <= data.GetEndFrequency() + 5e6 )
        {
            error = 0;

            // Calculate the associated error to the current value of alpha
            for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = pts.GetSetOfPoints(0).begin(); it != pts.GetSetOfPoints(0).end(); ++it )
            {
                error -= Gaus_Eval( it->second.fOrdinate - q * it->second.fAbscissa - alpha, fProbeWidthBig );
            }
            
            // Add point
            newData.AddPoint( alpha - (data.GetStartFrequency() - 5e6), KTPowerFitData::Point( alpha - (data.GetStartFrequency() - 5e6), -1*error, pts.GetSetOfPoints(0).begin()->second.fThreshold) );
            KTDEBUG(sdlog, "Added point of intercept " << alpha - (data.GetStartFrequency() - 5e6) << " and error " << -1 * error);
            
            // Increment alpha
            alpha += fStepSizeBig;
        }

        KTINFO(sdlog, "Sucessfully gathered points for power fit calculation. Performing fit...");

        TH1D* fitPoints = KT2ROOT::CreateMagnitudeHistogram( &newData, "hPowerMag" );
        TF1* conv = new TF1( "conv", fitf, 0, 1e9, 5 );
        TF1* gaussian = new TF1( "gaussian", "gaus(0) + [3]", 0, 1e9);

        conv->SetParameter( 0, 1 );
        conv->SetParameter( 1, 40 );
        conv->SetParameter( 2, 5 );
        conv->SetParameter( 3, 0.5 );
        conv->SetParameter( 4, 100e3 );

        conv->SetParLimits( 3, 0.1, TMath::Pi() );
        conv->SetParLimits( 4, 0, 10e6 );

        TFitResultPtr fitStatus = fitPoints->Fit( "conv", "S" );
        
        double a = conv->GetParameter( 0 );
        double a_err = conv->GetParError( 0 );

        double b = conv->GetParameter( 1 );
        double b_err = conv->GetParError( 1 );

        double z0 = conv->GetParameter( 2 );
        double z0_err = conv->GetParError( 2 );

        double k = conv->GetParameter( 3 );
        double k_err = conv->GetParError( 3 );

        double sigma = conv->GetParameter( 4 );
        double sigma_err = conv->GetParError( 4 );

        bool valid = fitStatus->IsValid();

        KTINFO(sdlog, "Completed fit! Validity = " << fitStatus->IsValid());

        newData.SetScale( a );
        newData.SetScaleErr( a_err );

        newData.SetBackground( b );
        newData.SetBackgroundErr( b_err );

        newData.SetCenter( z0 );
        newData.SetCenterErr( z0_err );

        newData.SetCurvature( k );
        newData.SetCurvatureErr( k_err );

        newData.SetWidth( sigma );
        newData.SetWidthErr( sigma_err );

        newData.SetIsValid( valid == 1 );

        return true;
    }

    bool KTLinearDensityProbeFit::PerformTest(KTDiscriminatedPoints2DData& pts, KTLinearFitResult& newData, double fProbeWidth, double fStepSize, unsigned component)
    {
        double alpha = fMinFrequency;
        double bestAlpha = 0, bestError = 0, error = 0;
        
        KTINFO(sdlog, "Performing density probe test with fProbeWidth = " << fProbeWidth << " and fStepSize = " << fStepSize);
        bestAlpha = findIntercept( pts, fStepSize, newData.GetSlope( component ), fProbeWidth );
        newData.SetIntercept( bestAlpha, component );

        return true;
    }
    

} /* namespace Katydid */
