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
#include "TSpectrum.h"
#include "TVirtualFitter.h"
#endif

using std::string;
using std::vector;


namespace Katydid
{
    KTLOGGER(evlog, "KTLinearDensityProbeFit");

    KT_REGISTER_PROCESSOR(KTLinearDensityProbeFit, "linear-density-fit");

    KTLinearDensityProbeFit::KTLinearDensityProbeFit(const std::string& name) :
            KTProcessor(name),
            fMinFrequency(0.),
            fMaxFrequency(1.),
            fProbeWidthBig(1e6),
            fProbeWidthSmall(20e3),
            fStepSize(25e3),
            fTolerance(5.),
            fThreshold(0.5),
            fAlgorithm(0),
            fLinearDensityFitSignal("density-fit", this),
            fPowerFitSignal("power-fit", this),
            fPreCalcSlot("gv", this, &KTLinearDensityProbeFit::SetPreCalcGainVar)
    {
        RegisterSlot( "thresh-points", this, &KTLinearDensityProbeFit::SlotFunctionThreshPoints );
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
        }
        if (node->has("probe-width-small"))
        {
            SetProbeWidthSmall(node->get_value< double >("probe-width-small"));
            SetStepSize(node->get_value< double >("probe-width-small") / 5);
        }

        SetStepSize(node->get_value< double >("step-size", fStepSize));
        SetAlgorithm(node->get_value< unsigned >("algorithm", fAlgorithm));

        return true;
    }

    bool KTLinearDensityProbeFit::ChooseAlgorithm( KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram )
    {
        // fAlgorithm == 1 will execute only the first conditional
        // fAlgorithm == 2 will execute only the second
        // Any other value will execute them both

        if( fAlgorithm != 2 )
        {
            if( !DensityMaximization( data, pts, fullSpectrogram ) )
            {
                KTERROR(evlog, "Something went wrong performing the density maximization algorithm!");
                return false;
            }
        }
        if( fAlgorithm != 1 )
        {
            if( !ProjectionAnalysis( data, pts, fullSpectrogram ) )
            {
                KTERROR(evlog, "Something went wrong performing the projection analysis algorithm!");
                return false;
            }
        }

        return true;
    }

    // function to evaluate gaussian function
    double Gaus_Eval( double arg, double sigma )
    {
        return exp( -1*pow(arg/sigma, 2)/2 );
    }

    // Determines the significance of a 1D peak in Sigma or SNR
    // x is a vector of 1D data
    // omit contains the indices of points to omit from noise calculation
    // include is the index of the point to calculate the significance of
    // metric determines whether to return a result in SNR or Sigma

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

    // Performs brute-force intercept sweep and returns the point of maximum density for specific q, width, and step size
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
            
            // Update bestError and bestAlpha if necessary
            // The first iteration will have bestError = 0 and must be updated regardless

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

    // Function to define a TF1
    // Sum of npeaks Gaussian peaks on a linear background
    // Background is specified by par[0] and par[1]
    // Gaussian peaks are specified by all other elements of par
    // npeaks must be declared here for use in both fpeaks and the ProjectionAnalysis method

    Int_t npeaks;
    Double_t fpeaks( Double_t *x, Double_t *par )
    {
        // The frequency domain will be in units of MHz to aid with the fit
        // This makes all of the parameters much closer to O(1) rather than having some O(1e8)

        // par[1] is the slope of the linear background, and has units 1/MHz
        // Thus it must be divided by 1e6
        // All mean and sigma parameters have units of MHz and thus must be multiplied by 1e6

        Double_t result = par[0] + par[1] * x[0] / 1e6;

        for( Int_t p = 0; p < npeaks; p++ )
        {
            Double_t norm  = par[3*p+2];
            Double_t mean  = par[3*p+3] * 1e6;
            Double_t sigma = par[3*p+4] * 1e6;

            result += norm * TMath::Gaus( x[0], mean, sigma );
        }

        return result;
    }

    // Main method for density maximization algorithm
    bool KTLinearDensityProbeFit::DensityMaximization(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram)
    {
        KTLinearFitResult& newData = data.Of< KTLinearFitResult >();

        // Set up the KTLinearFitResult object with some data from the track
        newData.SetNComponents( 2 );
        newData.SetSlope( data.GetSlope(), 0 );
        newData.SetSlope( data.GetSlope(), 1 );
        newData.SetTrackDuration( data.GetEndTimeInRunC() - data.GetStartTimeInRunC(), 0 );
        newData.SetTrackDuration( data.GetEndTimeInRunC() - data.GetStartTimeInRunC(), 1 );

        // Perform the brute-force intercept sweeps
        PerformTest( pts, newData, fProbeWidthBig, fStepSize, 0 );
        PerformTest( pts, newData, fProbeWidthSmall, fStepSize, 1 );

        double intercept1 = newData.GetIntercept( 0 );
        double intercept2 = newData.GetIntercept( 1 );

        KTDEBUG(evlog, "Signal candidate intercept = " << intercept2 );
        KTDEBUG(evlog, "Sideband candidate intercept = " << intercept1 );

        // Update the result with the separation between intercepts
        newData.SetSidebandSeparation( intercept1 - intercept2, 0 );
        newData.SetSidebandSeparation( intercept1 - intercept2, 1 );

        KTINFO(evlog, "Found best-fit intercepts. Continuing with fourier analysis of sideband candidate");

        // This procedure follows from Noah's analysis of the weighted and unweighted projections
        // Details here: https://basecamp.com/1780990/projects/338746/messages/46624702

        // We will need to calculate the unweighted projection first
        // I've arbitrarily picked a window which is +/- 1MHz from the intercept

        double alphaBound_upper = intercept1 + 1e6;
        double alphaBound_lower = intercept1 - 1e6;

        // Window and spectrogram parameters
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
        KTINFO(evlog, "Set xBin range to " << xBinStart << ", " << xBinEnd);

        newData.SetFit_width( xWindow, 0 );
        newData.SetFit_width( xWindow, 1 );

        // The y window this time will be floating, but its size will be consistent
        yWindow = ceil( (alphaBound_upper - alphaBound_lower) / ps_dy );

        // Vectors to hold the unweighted projection, weighted projection, and fourier transform of the weighted projection
        vector< double > unweighted, weighted, fourier;

        double delta_f;
        int i = 0;

        KTDEBUG(evlog, "Computing unweighted projection");

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

        KTDEBUG(evlog, "Computing weighted projection");

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

        // Evaluate SNR and Sigma significance of the largest fourier peak
        // This peak analysis is really really bad, and should be improved. The other algorithm is more recent and
        // uses TSpectrum, which we may also want to implement here

        double avg_fourier = 0.;
        double max_fourier = 0.;
        double freq_step = 1/(2 * (xWindow - 1) * ps_dx);

        for( int k = 0; k < xWindow; k++ )
        {
            avg_fourier += fourier[k] / xWindow;
        }

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

        KTINFO(evlog, "Successfully obtained power modulation. Maximum fourier peak at frequency " << newData.GetFFT_peak( 0 ) << " with SNR " << newData.GetFFT_SNR( 0 ));

        return true;
    }

    // Main method for power projection algorithm
    bool KTLinearDensityProbeFit::ProjectionAnalysis(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram)
    {
        KTPowerFitData& newData = data.Of< KTPowerFitData >();
        newData.SetNComponents( 1 );

        double density, alpha;
        double q = data.GetSlope();

        // Intercept range is determined by the spectrogram window
        double minAlpha = fullSpectrogram.GetMinFreq() - q * fullSpectrogram.GetStartTime();
        double maxAlpha = fullSpectrogram.GetMaxFreq() - q * fullSpectrogram.GetEndTime();

        // Begin brute-force sweep
        alpha = minAlpha;
        while( alpha <= maxAlpha )
        {
            density = 0;

            // Calculate the density associated to the current value of alpha
            for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = pts.GetSetOfPoints(0).begin(); it != pts.GetSetOfPoints(0).end(); ++it )
            {
                density += Gaus_Eval( it->second.fOrdinate - q * it->second.fAbscissa - alpha, fProbeWidthSmall );
            }
            
            // Add point to the KTPowerFitData
            newData.AddPoint( alpha, KTPowerFitData::Point( alpha, density, pts.GetSetOfPoints(0).begin()->second.fThreshold) );
            KTDEBUG(evlog, "Added point of intercept " << alpha << " and density " << density);
            
            // Increment alpha
            alpha += fStepSize;
        }

        KTINFO(evlog, "Sucessfully gathered points for peak finding analysis");

        // Create histogram from the sweep results
        TH1D* fitPoints = KT2ROOT::CreateMagnitudeHistogram( &newData, "hPowerMag" );
 
        // The peak finding analysis uses TSpectrum
        // It is adapted from an example script written by Rene Brun: https://root.cern.ch/root/html/tutorials/spectrum/peaks.C.html
        // The search tolerance and threshold are configurable parameters

        TSpectrum *s = new TSpectrum::TSpectrum( 10 ); // Maximum number of peaks = 10
        Int_t nfound = s->Search( fitPoints, fTolerance, "", fThreshold );

        KTINFO(evlog, "Found " << nfound << " candidate peaks to fit");

        // Estimate background using TSpectrum::Background
        TH1 *hb = s->Background( fitPoints, 20, "same" );

        // Estimate linear background using a fitting method
        TF1 *fline = new TF1( "fline", "pol1", 0, 1e9 );
        fitPoints->Fit( "fline", "qn" );

        // Maximum 10 peaks = 32 parameters (3*10 + 2)
        Double_t par[32];

        par[0] = fline->GetParameter( 0 );
        par[1] = fline->GetParameter( 1 );

        // Loop on all found peaks
        // Peaks with SNR < 2 will be discarded

        npeaks = 0;
        Double_t *xpeaks = s->GetPositionX();

        for( Int_t p = 0; p < nfound; p++ )
        {
            // Acquire peak location and bin
            Double_t xp = xpeaks[p];
            Int_t bin = fitPoints->GetXaxis()->FindBin( xp );
            Double_t yp = fitPoints->GetBinContent( bin );

            KTINFO(evlog, "Looking at peak (" << xp << ", " << yp << ")");

            // If the y-position is less than twice the background level (SNR < 2), we will discard it
            if( yp < 2 * fline->Eval( xp ) )
            {
                KTDEBUG(evlog, "Discarding peak");
                continue;
            }

            KTDEBUG(evlog, "Keeping peak");

            // If it is kept, add the info to par
            par[3*npeaks+2] = yp;
            par[3*npeaks+3] = xp / 1e6;
            par[3*npeaks+4] = 0.1; // Arbitrary default guess of 100kHz width

            // Increment npeaks
            npeaks++;
        }

        KTINFO(evlog, "Found " << npeaks << " useful peaks to fit");
        KTINFO(evlog, "Now fitting: Be patient...");

        // Next we set up a TF1 to fit the power projection
        // Recall fpeaks is a sum of npeaks Gaussians

        TF1 *fit = new TF1( "fit", fpeaks, 0, 1e9, 2+3*npeaks );

        // We may have more than the default 25 parameters
        TVirtualFitter::Fitter( fitPoints, 10+3*npeaks );

        // Set guess parameters and limits
        // We will insist that all the Gaussian parameters are positive
        // The upper limits are arbitrary but much larger than anything sensible

        fit->SetParameters( par );
        for( Int_t p = 0; p < npeaks; p++ )
        {
            fit->SetParLimits( 3*p+2, 0, 1000 );
            fit->SetParLimits( 3*p+3, 0, 50000 );
            fit->SetParLimits( 3*p+4, 0, 100 );
        }
        fit->SetNpx( 1000 );

        // Perform fit
        TFitResultPtr fitStatus = fitPoints->Fit( "fit", "S" );

        KTINFO(evlog, "Fit completed!");

        // Finally we put all of the fit information into the KTPowerFitData object
        // Vectors will hold the parameters and associated errors

        std::vector<double> norms, means, sigmas, maxima;
        std::vector<double> normErrs, meanErrs, sigmaErrs, maximumErrs;

        double invsqrt2pi = TMath::Power( 2*TMath::Pi(), -0.5 ); // I could have just written the number but eh

        // Loop over found peaks and fill the vectors
        for( Int_t p = 0; p < npeaks; p++ )
        {
            norms.push_back( fit->GetParameter(3*p+2) );
            means.push_back( fit->GetParameter(3*p+3) );
            sigmas.push_back( fit->GetParameter(3*p+4) );
            maxima.push_back( invsqrt2pi * fit->GetParameter(3*p+2) / fit->GetParameter(3*p+4) );

            normErrs.push_back( fit->GetParError(3*p+2) );
            meanErrs.push_back( fit->GetParError(3*p+3) );
            sigmaErrs.push_back( fit->GetParError(3*p+4) );

            // Really annoying error propagation for this one
            maximumErrs.push_back( TMath::Sqrt( TMath::Power( invsqrt2pi / fit->GetParameter(3*p+4) * fit->GetParError(3*p+2), 2 ) + TMath::Power( invsqrt2pi * fit->GetParameter(3*p+2) / TMath::Power( fit->GetParameter(3*p+4), 2 ) * fit->GetParError(3*p+4), 2 ) ) );
        }

        // We will also store the validity of the fit
        bool valid = fitStatus->IsValid();
        KTINFO(evlog, "Fit validity = " << valid);

        // Fill newData variables
        newData.SetNorm( norms );
        newData.SetMean( means );
        newData.SetSigma( sigmas );
        newData.SetMaximum( maxima );

        newData.SetNormErr( normErrs );
        newData.SetMeanErr( meanErrs );
        newData.SetSigmaErr( sigmaErrs );
        newData.SetMaximumErr( maximumErrs );

        // Validity
        if( valid )
        {
            newData.SetIsValid( 1 );
        }
        else
        {
            newData.SetIsValid( 0 );
        }

        // For post-analysis purposes I've included a frequency cut to identify main peaks and sidebands
        // This **SHOULD NOT** be used as a meaningful analysis result!!
        // It is also specific to a particular data set, so if you're not me you probably shouldn't use it at all

        if( data.GetStartFrequency() > 75e6 && data.GetStartFrequency() < 115e6 )
        {
            newData.SetMainPeak( 1 );
        }
        else
        {
            newData.SetMainPeak( 0 );
        }

        // npeaks
        newData.SetNPeaks( npeaks );

        // Calculate first four moments from TH1 directly
        newData.SetAverage( fitPoints->GetMean() );
        newData.SetRMS( fitPoints->GetRMS() );
        newData.SetSkewness( fitPoints->GetSkewness() );
        newData.SetKurtosis( fitPoints->GetKurtosis() );

        return true;
    }

    bool KTLinearDensityProbeFit::PerformTest(KTDiscriminatedPoints2DData& pts, KTLinearFitResult& newData, double fProbeWidth, double fStepSize, unsigned component)
    {
        double alpha = fMinFrequency;
        double bestAlpha = 0, bestError = 0, error = 0;
        
        KTINFO(evlog, "Performing density probe test with fProbeWidth = " << fProbeWidth << " and fStepSize = " << fStepSize);
        bestAlpha = findIntercept( pts, fStepSize, newData.GetSlope( component ), fProbeWidth );
        newData.SetIntercept( bestAlpha, component );

        return true;
    }
    

} /* namespace Katydid */
