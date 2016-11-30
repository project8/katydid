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
#include "KTMath.hh"

#include <cmath>
#include <vector>
#include <algorithm>

#ifdef USE_OPENMP
#include <omp.h>
#endif

#include "TH1.h"
#include "TF1.h"
#include "KT2ROOT.hh"
#include "TMath.h"
#include "TFitResult.h"
#include "TSpectrum.h"
#include "TVirtualFitter.h"

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
            if( ! DensityMaximization( data, pts, fullSpectrogram ) )
            {
                KTERROR(evlog, "Something went wrong performing the density maximization algorithm!");
                return false;
            }
        }
        if( fAlgorithm != 1 )
        {
            if( ! ProjectionAnalysis( data, pts, fullSpectrogram ) )
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
        return exp( -0.5*arg*arg/(sigma*sigma) );
    }

    // Determines the significance of a 1D peak in Sigma or SNR
    // x is a vector of 1D data
    // omit contains the indices of points to omit from noise calculation
    // include is the index of the point to calculate the significance of
    // metric determines whether to return a result in SNR or Sigma

    // This function is pretty terrible and not used, so I'm just going to comment it out
    // Perhaps at some point it can be made better, and used, and uncommented
/*
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
*/
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
    // npeaks is initialized in the header file

    double SumOfGaussians( double *x, double *par )
    {
        // The frequency domain will be in units of MHz to aid with the fit
        // This makes all of the parameters much closer to O(1) rather than having some O(1e8)

        // par[1] is the slope of the linear background, and has units 1/MHz
        // Thus it must be divided by 1e6
        // All mean and sigma parameters have units of MHz and thus must be multiplied by 1e6

        // We will assume the number of peaks is the maximum npeaks = 10

        double result = par[0] + par[1] * x[0] * 1e-6;

        for( int peak = 0; peak < 10; ++peak )
        {
            double norm  = par[3*peak+2];
            double mean  = par[3*peak+3] * 1e6;
            double sigma = par[3*peak+4] * 1e6;

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

        // Calculate number of points
        int nPts = pts.GetSetOfPoints(0).size();
        newData.SetNPoints( nPts, 0 );
        newData.SetNPoints( nPts, 1 );

        // Perform the brute-force intercept sweeps
        PerformTest( pts, newData, fProbeWidthBig, fStepSize, 0 );
        PerformTest( pts, newData, fProbeWidthSmall, fStepSize, 1 );

        newData.SetProbeWidth( fProbeWidthBig, 0 );
        newData.SetProbeWidth( fProbeWidthSmall, 1 );

        double intercept1 = newData.GetIntercept( 0 );
        double intercept2 = newData.GetIntercept( 1 );

        KTDEBUG(evlog, "Signal candidate intercept = " << intercept2 );
        KTDEBUG(evlog, "Sideband candidate intercept = " << intercept1 );

        // Update the result with start frequencies and the separation between intercepts
        newData.SetSidebandSeparation( intercept1 - intercept2, 0 );
        newData.SetSidebandSeparation( intercept1 - intercept2, 1 );

        newData.SetStartingFrequency( data.GetStartFrequency() - data.GetIntercept() + intercept1, 0 );
        newData.SetStartingFrequency( data.GetStartFrequency() - data.GetIntercept() + intercept2, 1 );

        KTINFO(evlog, "Found best-fit intercepts. Continuing with fourier analysis of sideband candidate");

        // This procedure follows from Noah's analysis of the weighted and unweighted projections
        // Details here: https://basecamp.com/1780990/projects/338746/messages/46624702

        // We will need to calculate the unweighted projection first
        // I've arbitrarily picked a window which is +/- 1MHz from the intercept

        double alphaBound_upper = intercept1 + 1e6;
        double alphaBound_lower = intercept1 - 1e6;

        // Window and spectrogram parameters

        double ps_xmin = fullSpectrogram.GetStartTime();
        double ps_xmax = fullSpectrogram.GetEndTime();
        double ps_ymin = fullSpectrogram.GetSpectra().begin()->second->GetRangeMin();
        //     ps_ymax will not be necessary
        double ps_dx   = fullSpectrogram.GetDeltaT();
        double ps_dy   = fullSpectrogram.GetSpectra().begin()->second->GetFrequencyBinWidth();
        
        // We add +1 for the underflow bin
        int xBinStart = floor( (data.GetStartTimeInRunC() - ps_xmin) / ps_dx ) + 1;
        int xBinEnd   = floor( (data.GetEndTimeInRunC() - ps_xmin) / ps_dx ) + 1;
        int xWindow = xBinEnd - xBinStart + 1;
        KTINFO(evlog, "Set xBin range to " << xBinStart << ", " << xBinEnd);

        int yBinStart = 0; // Will be set during the projection calculatinos
        //  yBinEnd will not be necessary
        int yWindow = ceil( (alphaBound_upper - alphaBound_lower) / ps_dy ); // The y window this time will be floating, but its size will be consistent

        double q_fit = newData.GetSlope( 0 );
        
        newData.SetFit_width( xWindow, 0 );
        newData.SetFit_width( xWindow, 1 );

        // Vectors to hold the unweighted projection, weighted projection, and fourier transform of the weighted projection
        vector< double > unweighted, weighted, fourier;

        double xVal, yVal; // time (x) and frequency (y) values to be incremented in the loops below
        double delta_f; // used in weighted projection calculated
        int iSpectrum = 0;

        KTDEBUG(evlog, "Computing unweighted projection");

        // First we compute the unweighted projection
        for( std::map< double, KTPowerSpectrum* >::const_iterator it = fullSpectrogram.GetSpectra().begin(); it != fullSpectrogram.GetSpectra().end(); ++it )
        {
            // Set x value and starting y-bin
            xVal = ps_xmin + (iSpectrum - 1) * ps_dx;
            yBinStart = it->second->FindBin( alphaBound_lower + q_fit * xVal );

            // Unweighted power = sum of raw power spectrum
            unweighted.push_back( 0 );
            for( int iBin = yBinStart; iBin < yBinStart + yWindow; ++iBin )
            {
                yVal = ps_ymin + ps_dy * (iBin - 1);

                // We reevaluate the spline rather than deal with the appropriate index of power_minus_bkgd
                unweighted[iSpectrum] += (*it->second)(iBin) - fGVData.GetSpline()->Evaluate( yVal );
            }
            ++iSpectrum;
        }

        KTDEBUG(evlog, "Computing weighted projection");

        iSpectrum = 0;

        // Weighted projection
        double cumulative;
        for( std::map< double, KTPowerSpectrum* >::const_iterator it = fullSpectrogram.GetSpectra().begin(); it != fullSpectrogram.GetSpectra().end(); ++it )
        {
            cumulative = 0.;

            xVal = ps_xmin + (iSpectrum - 1) * ps_dx;
            yBinStart = it->second->FindBin( alphaBound_lower + q_fit * xVal );

            for( int iBin = yBinStart; iBin < yBinStart + yWindow; ++iBin )
            {
                yVal = ps_ymin + ps_dy * (iBin - 1);

                // Calculate delta-f using the fit values
                delta_f = yVal - (q_fit * xVal + newData.GetIntercept(0));
                cumulative += delta_f * ((*it->second)(iBin) - fGVData.GetSpline()->Evaluate( yVal )) / unweighted[iSpectrum];
            }

            weighted.push_back( cumulative );
            ++iSpectrum;
        }

        // Discrete Cosine Transform (real -> real) of type I
        // Explicit, not fast (i.e. n^2 operations)

        for( int xBin = 0; xBin < xWindow; ++xBin )
        {
            cumulative = 0.;
            for( int xxBin = 1; xxBin <= xWindow - 2; ++xxBin )
                cumulative += weighted[xxBin] * cos( xxBin * xBin * KTMath::Pi() / (xWindow - 1) );

            fourier.push_back( pow( 0.5 * (weighted[0] + pow( -1, xBin ) * weighted[xWindow - 1]) + cumulative, 2 ) );
        }

        // Evaluate SNR and Sigma significance of the largest fourier peak
        // This peak analysis is really really bad, and should be improved. The other algorithm is more recent and
        // uses TSpectrum, which we may also want to implement here

        double avg_fourier = 0.;
        double max_fourier = 0.;
        double freq_step = 1/(2 * (xWindow - 1) * ps_dx);

        for( int xBin = 0; xBin < xWindow; ++xBin )
        {
            avg_fourier += fourier[xBin] / xWindow;
        }

        for( int xBin = 0; xBin < xWindow; ++xBin )
        {
            if( fourier[xBin] > max_fourier )
            {
                max_fourier = fourier[xBin];
                newData.SetFFT_peak( double(xBin) * freq_step, 0 );
                newData.SetFFT_SNR( max_fourier / avg_fourier, 0 );
                newData.SetFFT_peak( double(xBin) * freq_step, 1 );
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

        TSpectrum *s = new TSpectrum( 10 ); // Maximum number of peaks = 10
        int nfound = s->Search( fitPoints, fTolerance, "", fThreshold );

        KTINFO(evlog, "Found " << nfound << " candidate peaks to fit");

        // Estimate background using TSpectrum::Background
        TH1 *hb = s->Background( fitPoints, 20, "same" );

        // Estimate linear background using a fitting method
        TF1 *fline = new TF1( "fline", "pol1", 0, 1e9 );
        fitPoints->Fit( "fline", "qn" );

        // Maximum 10 peaks = 32 parameters (3*10 + 2)
        double par[32];

        par[0] = fline->GetParameter( 0 );
        par[1] = fline->GetParameter( 1 );

        // Loop on all found peaks
        // Peaks with SNR < 2 will be discarded

        npeaks = 0;
        double *xpeaks = s->GetPositionX();

        for( int peak = 0; peak < nfound; ++peak )
        {
            // Acquire peak location and bin
            double xp = xpeaks[peak];
            int bin = fitPoints->GetXaxis()->FindBin( xp );
            double yp = fitPoints->GetBinContent( bin );

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
            ++npeaks;
        }

        KTINFO(evlog, "Found " << npeaks << " useful peaks to fit");
        KTINFO(evlog, "Now fitting: Be patient...");

        // Next we set up a TF1 to fit the power projection
        // Recall SumOfGaussians is a sum of npeaks Gaussians

        TF1 *fit = new TF1( "fit", SumOfGaussians, 0, 1e9, 32 );

        // We may have more than the default 25 parameters
        TVirtualFitter::Fitter( fitPoints, 10+3*npeaks );

        // Set guess parameters and limits
        // We will insist that all the Gaussian parameters are positive
        // The upper limits are arbitrary but much larger than anything sensible

        fit->SetParameters( par );
        for( int peak = 0; peak < npeaks; ++peak )
        {
            fit->SetParLimits( 3*peak+2, 0, 1000 );
            fit->SetParLimits( 3*peak+3, 0, 50000 );
            fit->SetParLimits( 3*peak+4, 0, 100 );
        }
        fit->SetNpx( 1000 );

        // Fix all parameters which will not be used because npeaks < 10
        for( int tooManyPeaks = 9; tooManyPeaks >= npeaks; tooManyPeaks-- )
        {
            fit->FixParameter( 3*tooManyPeaks+2, 0 ); // norm = 0 should ensure no contribution
            fit->FixParameter( 3*tooManyPeaks+3, 0 );
            fit->FixParameter( 3*tooManyPeaks+4, 1 ); // just to avoid 0/0
        }

        // Perform fit
        TFitResultPtr fitStatus = fitPoints->Fit( "fit", "S" );

        KTINFO(evlog, "Fit completed!");

        // Finally we put all of the fit information into the KTPowerFitData object
        // Vectors will hold the parameters and associated errors

        std::vector<double> norms, means, sigmas, maxima;
        std::vector<double> normErrs, meanErrs, sigmaErrs, maximumErrs;

        double invsqrt2pi = TMath::Power( 2*KTMath::Pi(), -0.5 ); // I could have just written the number but eh

        // Loop over found peaks and fill the vectors
        for( int peak = 0; peak < npeaks; ++peak )
        {
            norms.push_back( fit->GetParameter(3*peak+2) );
            means.push_back( fit->GetParameter(3*peak+3) );
            sigmas.push_back( fit->GetParameter(3*peak+4) );
            //maxima.push_back( invsqrt2pi * fit->GetParameter(3*p+2) / fit->GetParameter(3*p+4) );
            maxima.push_back( s->GetPositionY()[peak] );

            normErrs.push_back( fit->GetParError(3*peak+2) );
            meanErrs.push_back( fit->GetParError(3*peak+3) );
            sigmaErrs.push_back( fit->GetParError(3*peak+4) );

            // Really annoying error propagation for this one
            maximumErrs.push_back( TMath::Sqrt( TMath::Power( invsqrt2pi / fit->GetParameter(3*peak+4) * fit->GetParError(3*peak+2), 2 ) + TMath::Power( invsqrt2pi * fit->GetParameter(3*peak+2) / TMath::Power( fit->GetParameter(3*peak+4), 2 ) * fit->GetParError(3*peak+4), 2 ) ) );
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

        if( data.GetStartFrequency() > 100e6 )
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
        double meanCorrection = 0.5 * (minAlpha + maxAlpha);
        newData.SetAverage( (fitPoints->GetMean() - meanCorrection)/1e6 );
        newData.SetRMS( fitPoints->GetRMS()/1e6 );
        newData.SetSkewness( fitPoints->GetSkewness() );
        newData.SetKurtosis( fitPoints->GetKurtosis() );

        // Calculate classifiers
        KTINFO(evlog, "Calculating classifiers for KTPowerFitData");

        // First find the most central peak
        int cpIndex;
        double cpLocation;
        for( int peak = 0; peak < npeaks; ++peak )
        {
            KTDEBUG(evlog, "Peak " << peak << " has location " << xpeaks[peak] - meanCorrection);
            if( peak == 0 || std::abs( xpeaks[peak] - meanCorrection ) < std::abs( cpLocation ) )
            {
                cpIndex = peak;
                cpLocation = xpeaks[peak] - meanCorrection;
            }
        }

        KTINFO(evlog, "Central peak has index " << cpIndex << " and location " << cpLocation);

        // Set central peak fit parameters
        newData.SetNormCentral( fit->GetParameter( 3*cpIndex+2 ) );
        newData.SetMeanCentral( fit->GetParameter( 3*cpIndex+3 ) - meanCorrection/1e6 );
        newData.SetSigmaCentral( fit->GetParameter( 3*cpIndex+4) );
        newData.SetMaximumCentral( invsqrt2pi * fit->GetParameter( 3*cpIndex+2 ) / fit->GetParameter( 3*cpIndex+4 ) );

        // Vectors to calculate statistics near and away from the central peak
        std::vector<double> centralPoints;
        std::vector<double> nonCentralPoints;

        // Points and iterator
        std::map< unsigned, KTPowerFitData::Point >::iterator it;
        std::map< unsigned, KTPowerFitData::Point > SetOfPoints = newData.GetSetOfPoints();

        // Iterate over all points and fill the appropriate vector
        int iBin = 1;
        for( it = SetOfPoints.begin(); it != SetOfPoints.end(); ++it )
        {
            if( it->second.fAbscissa - meanCorrection >= newData.GetMeanCentral()*1e6 - 3 * newData.GetSigmaCentral()*1e6 && it->second.fAbscissa - meanCorrection <= newData.GetMeanCentral()*1e6 + 3 * newData.GetSigmaCentral()*1e6 )
            {
                centralPoints.push_back( it->second.fOrdinate );
                KTDEBUG(evlog, "Added point at " << it->second.fAbscissa - meanCorrection << " to centralPoints vector");
            }
            else
            {
                nonCentralPoints.push_back( it->second.fOrdinate );
                KTDEBUG(evlog, "Added point at " << it->second.fAbscissa - meanCorrection << " to nonCentralPoints vector");
            }
            ++iBin;
        }
        
        // Initialize variables for mean and RMS
        double centralMean = 0;
        double centralRMS = 0;
        double nonCentralMean = 0;
        double nonCentralRMS = 0;

        // Calculate central mean and RMS
        for( int iPoint = 0; iPoint < centralPoints.size(); ++iPoint )
        {
            centralMean += centralPoints.at(iPoint);
            centralRMS += TMath::Power( centralPoints.at(iPoint), 2 );
        }
        centralMean /= (double)(centralPoints.size());
        centralRMS /= (double)(centralPoints.size());
        centralRMS = TMath::Power( centralRMS - TMath::Power( centralMean, 2 ), 0.5 );

        KTINFO(evlog, "Calculated central mean = " << centralMean << " and RMS = " << centralRMS);

        // Calculate non-central mean and RMS
        for( int iPoint = 0; iPoint < nonCentralPoints.size(); ++iPoint )
        {
            nonCentralMean += nonCentralPoints.at(iPoint);
            nonCentralRMS += TMath::Power( nonCentralPoints.at(iPoint), 2 );
        }
        nonCentralMean /= (double)(nonCentralPoints.size());
        nonCentralRMS /= (double)(nonCentralPoints.size());
        nonCentralRMS = TMath::Power( nonCentralRMS - TMath::Power( nonCentralMean, 2 ), 0.5 );

        KTINFO(evlog, "Calculated non-central mean = " << nonCentralMean << " and RMS = " << nonCentralRMS);

        // Fill data
        newData.SetRMSAwayFromCentral( nonCentralRMS );
        newData.SetCentralPowerRatio( centralMean / nonCentralMean );

        // Lastly we copy the track intercept to newData
        newData.SetTrackIntercept( data.GetIntercept() );

        KTINFO(evlog, "Finished classifier calculations. Power fit data is done!");


        // *****The following is only for Gray's analysis*****


        // Vectors to calculate statistics near and away from the central peak
        centralPoints.clear();
        nonCentralPoints.clear();

        // Iterate over all points and fill the appropriate vector
        iBin = 1;
        for( it = SetOfPoints.begin(); it != SetOfPoints.end(); ++it )
        {
            if( it->second.fAbscissa - meanCorrection >= -1e6 && it->second.fAbscissa - meanCorrection <= 1e6 )
            {
                centralPoints.push_back( it->second.fOrdinate );
                KTDEBUG(evlog, "Added point at " << it->second.fAbscissa - meanCorrection << " to centralPoints vector");
            }
            else if( it->second.fAbscissa - meanCorrection >= -10e6 && it->second.fAbscissa - meanCorrection <= 10e6 )
            {
                nonCentralPoints.push_back( it->second.fOrdinate );
                KTDEBUG(evlog, "Added point at " << it->second.fAbscissa - meanCorrection << " to nonCentralPoints vector");
            }
            ++iBin;
        }
        
        // Reset variables for mean and RMS
        centralMean = 0;
        centralRMS = 0;
        nonCentralMean = 0;
        nonCentralRMS = 0;

        // Calculate central mean and RMS
        for( int iPoint = 0; iPoint < centralPoints.size(); ++iPoint )
        {
            centralMean += centralPoints.at(iPoint);
            centralRMS += TMath::Power( centralPoints.at(iPoint), 2 );
        }
        centralMean /= (double)(centralPoints.size());
        centralRMS /= (double)(centralPoints.size());
        centralRMS = TMath::Power( centralRMS - TMath::Power( centralMean, 2 ), 0.5 );

        KTINFO(evlog, "Calculated central mean = " << centralMean << " and RMS = " << centralRMS);

        // Calculate non-central mean and RMS
        for( int iPoint = 0; iPoint < nonCentralPoints.size(); ++iPoint )
        {
            nonCentralMean += nonCentralPoints.at(iPoint);
            nonCentralRMS += TMath::Power( nonCentralPoints.at(iPoint), 2 );
        }
        nonCentralMean /= (double)(nonCentralPoints.size());
        nonCentralRMS /= (double)(nonCentralPoints.size());
        nonCentralRMS = TMath::Power( nonCentralRMS - TMath::Power( nonCentralMean, 2 ), 0.5 );

        KTINFO(evlog, "Calculated non-central mean = " << nonCentralMean << " and RMS = " << nonCentralRMS);

        // Fill data
        newData.SetRMSAwayFromZero( nonCentralRMS );
        newData.SetZeroPowerRatio( centralMean / nonCentralMean );

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
