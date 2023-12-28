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
#include "logger.hh"
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
    LOGGER(evlog, "KTLinearDensityProbeFit");

    KT_REGISTER_PROCESSOR(KTLinearDensityProbeFit, "linear-density-fit");


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


    KTLinearDensityProbeFit::KTLinearDensityProbeFit(const std::string& name) :
                    KTProcessor(name),
                    fDoDensityMaximization(true),
                    fDoProjectionAnalysis(true),
                    fMinFrequency(0.),
                    fMaxFrequency(1.),
                    fProbeWidthBig(1e6),
                    fProbeWidthSmall(20e3),
                    fStepSize(25e3),
                    fTolerance(5.),
                    fThreshold(0.5),
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

        SetDoDensityMaximization(node->get_value("do-density-maximization", fDoDensityMaximization));
        SetDoProjectionAnalysis(node->get_value("do-projection-analysis", fDoProjectionAnalysis));
        SetMinFrequency(node->get_value("min-frequency", fMinFrequency));
        SetMaxFrequency(node->get_value("max-frequency", fMaxFrequency));
        SetProbeWidthBig(node->get_value("probe-width-big", fProbeWidthBig));
        if (node->has("probe-width-small"))
        {
            SetProbeWidthSmall(node->get_value< double >("probe-width-small"));
            SetStepSize(GetProbeWidthSmall() / 5.);
        }
        SetStepSize(node->get_value< double >("step-size", fStepSize));

        return true;
    }

    bool KTLinearDensityProbeFit::ChooseAlgorithm( KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram )
    {
        if (fDoDensityMaximization && ! DensityMaximization(data, pts, fullSpectrogram))
        {
            LERROR(evlog, "Something went wrong performing the density maximization algorithm!");
            return false;
        }
        if (fDoProjectionAnalysis && ! ProjectionAnalysis(data, pts, fullSpectrogram))
        {
            LERROR(evlog, "Something went wrong performing the projection analysis algorithm!");
            return false;
        }

        return true;
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
    double KTLinearDensityProbeFit::FindIntercept( KTDiscriminatedPoints2DData& pts, double dalpha, double q, double width )
    {
        double alpha = fMinFrequency;
        double bestAlpha = 0., bestError = 0.;
        while( alpha <= fMaxFrequency )
        {
            double error = 0.;

            // Calculate the associated error to the current value of alpha
            for( KTDiscriminatedPoints2DData::SetOfPoints::const_iterator it = pts.GetSetOfPoints(0).begin(); it != pts.GetSetOfPoints(0).end(); ++it )
            {
                error -= GausEval( it->second.fOrdinate - q * it->second.fAbscissa - alpha, width );
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


    // Main method for density maximization algorithm
    bool KTLinearDensityProbeFit::DensityMaximization(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram)
    {
        KTLinearFitResult& newData = data.Of< KTLinearFitResult >();

        unsigned trackComponent = data.GetComponent();

        // Set up the KTLinearFitResult object with some data from the track
        newData.SetComponent( data.GetComponent() );
        newData.SetNFits( 2 );
        newData.SetSlope( data.GetSlope(), 0 );
        newData.SetSlope( data.GetSlope(), 1 );
        newData.SetTrackDuration( data.GetTimeLength(), 0 );
        newData.SetTrackDuration( data.GetTimeLength(), 1 );

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

        LDEBUG(evlog, "Signal candidate intercept = " << intercept2 );
        LDEBUG(evlog, "Sideband candidate intercept = " << intercept1 );

        if( intercept1 == 0 || intercept2 == 0 )
        {
            LERROR(evlog, "Failed to find a suitable intercept. Perhaps there is a problem with fMin/MaxFrequency or the timestamps of your tracks. Aborting.");
            return false;
        }

        // Update the result with start frequencies and the separation between intercepts
        newData.SetSidebandSeparation( intercept1 - intercept2, 0 );
        newData.SetSidebandSeparation( intercept1 - intercept2, 1 );

        newData.SetStartingFrequency( data.GetStartFrequency() - data.GetIntercept() + intercept1, 0 );
        newData.SetStartingFrequency( data.GetStartFrequency() - data.GetIntercept() + intercept2, 1 );

        LINFO(evlog, "Found best-fit intercepts. Continuing with fourier analysis of sideband candidate");

        // This procedure follows from Noah's analysis of the weighted and unweighted projections
        // Details here: https://basecamp.com/1780990/projects/338746/messages/46624702

        // We will need to calculate the unweighted projection first
        // I've arbitrarily picked a window which is +/- 1MHz from the intercept

        double alphaBoundUpper = intercept1 + 1.e6;
        double alphaBoundLower = intercept1 - 1.e6;

        // Window and spectrogram parameters

        double ps_xmin = fullSpectrogram.GetStartTime();
        double ps_xmax = fullSpectrogram.GetEndTime();
        double ps_ymin = (*fullSpectrogram.GetSpectra())(0)->GetRangeMin();
        //     ps_ymax will not be necessary
        double ps_dx   = fullSpectrogram.GetDeltaT();
        double ps_dy   = (*fullSpectrogram.GetSpectra())(0)->GetFrequencyBinWidth();

        // We add +1 for the underflow bin
        int xBinStart = floor( (data.GetStartTimeInAcq() - ps_xmin) / ps_dx ) + 1;
        int xBinEnd   = floor( (data.GetStartTimeInAcq() + data.GetTimeLength() - ps_xmin) / ps_dx ) + 1;
        int xWindow = xBinEnd - xBinStart + 1;
        LDEBUG(evlog, "Set xBin range to " << xBinStart << ", " << xBinEnd);

        int yBinStart = 0; // Will be set during the projection calculations
        //  yBinEnd will not be necessary
        int yWindow = ceil( (alphaBoundUpper - alphaBoundLower) / ps_dy ); // The y window this time will be floating, but its size will be consistent

        double q_fit = newData.GetSlope( 0 );

        newData.SetFit_width( xWindow, 0 );
        newData.SetFit_width( xWindow, 1 );

        double xVal = 0., yVal = 0.; // time (x) and frequency (y) values to be incremented in the loops below
        double delta_f = 0.; // used in weighted projection calculated
        int iSpectrum = 0;

        LDEBUG(evlog, "Computing unweighted projection");

        KTSpline* spline = fGVData.GetSpline(trackComponent);

        // First we compute the unweighted projection
        const KTPhysicalArray< 1, KTPowerSpectrum* > spectra = *fullSpectrogram.GetSpectra();
        vector< double > unweighted(spectra.size());
        for( KTPhysicalArray< 1, KTPowerSpectrum* >::const_iterator it = spectra.begin(); it != spectra.end(); ++it )
        {
            // Set x value and starting y-bin
            xVal = ps_xmin + (iSpectrum - 1) * ps_dx;
            yBinStart = (*it)->FindBin( alphaBoundLower + q_fit * xVal );

            // Unweighted power = sum of raw power spectrum
            unweighted[iSpectrum] = 0;
            for( int iBin = yBinStart; iBin < yBinStart + yWindow && iBin < (*it)->GetNFrequencyBins(); ++iBin )
            {
                yVal = ps_ymin + ps_dy * (iBin - 1);

                // We reevaluate the spline rather than deal with the appropriate index of power_minus_bkgd
                unweighted[iSpectrum] += (**it)(iBin) - spline->Evaluate( yVal );
            }
            ++iSpectrum;
        }

        LDEBUG(evlog, "Computing weighted projection");

        // Weighted projection
        double cumulative = 0.;
        iSpectrum = 0;
        vector< double > weighted(spectra.size());
        for( KTPhysicalArray< 1, KTPowerSpectrum* >::const_iterator it = spectra.begin(); it != spectra.end(); ++it )
        {
            cumulative = 0.;

            xVal = ps_xmin + (iSpectrum - 1) * ps_dx;
            yBinStart = (*it)->FindBin( alphaBoundLower + q_fit * xVal );

            for( int iBin = yBinStart; iBin < yBinStart + yWindow && iBin < (*it)->GetNFrequencyBins(); ++iBin )
            {
                yVal = ps_ymin + ps_dy * (iBin - 1);

                // Calculate delta-f using the fit values
                delta_f = yVal - (q_fit * xVal + newData.GetIntercept(0));
                cumulative += delta_f * ((**it)(iBin) - spline->Evaluate( yVal )) / unweighted[iSpectrum];
            }

            weighted[iSpectrum] = cumulative;
            ++iSpectrum;
        }

        // Discrete Cosine Transform (real -> real) of type I
        // Explicit, not fast (i.e. n^2 operations)
        vector< double > fourier(xWindow);
        int sign = 1;
        for( int xBin = 0; xBin < xWindow; ++xBin )
        {
            cumulative = 0.;
            for( int xxBin = 1; xxBin <= xWindow - 2; ++xxBin )
            {
                cumulative += weighted[xxBin] * cos( xxBin * xBin * KTMath::Pi() / (xWindow - 1) );
            }

            double temp = 0.5 * (weighted[0] + double(sign) * weighted[xWindow - 1]) + cumulative;
            fourier[xBin] = temp * temp;
            sign *= -1;
        }

        // Evaluate SNR and Sigma significance of the largest fourier peak
        // This peak analysis is really really bad, and should be improved. The other algorithm is more recent and
        // uses TSpectrum, which we may also want to implement here

        double avg_fourier = 0.;
        double max_fourier = 0.;
        double freq_step = 1. / (2. * (xWindow - 1.) * ps_dx);
        double invXWindow = 1. / double(xWindow);

        for( int xBin = 0; xBin < xWindow; ++xBin )
        {
            avg_fourier += fourier[xBin] * invXWindow;
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

        LINFO(evlog, "Successfully obtained power modulation. Maximum fourier peak at frequency " << newData.GetFFT_peak( 0 ) << " with SNR " << newData.GetFFT_SNR( 0 ));

        return true;
    }

    // Main method for power projection algorithm
    bool KTLinearDensityProbeFit::ProjectionAnalysis(KTProcessedTrackData& data, KTDiscriminatedPoints2DData& pts, KTPSCollectionData& fullSpectrogram)
    {
        KTPowerFitData& newData = data.Of< KTPowerFitData >();

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
                density += GausEval( it->second.fOrdinate - q * it->second.fAbscissa - alpha, fProbeWidthSmall );
            }

            // Add point to the KTPowerFitData
            newData.AddPoint( alpha, KTPowerFitData::Point( alpha, density, pts.GetSetOfPoints(0).begin()->second.fThreshold) );
            LDEBUG(evlog, "Added point of intercept " << alpha << " and density " << density);

            // Increment alpha
            alpha += fStepSize;
        }

        LINFO(evlog, "Sucessfully gathered points for peak finding analysis");

        // Create histogram from the sweep results
        TH1D* fitPoints = KT2ROOT::CreateMagnitudeHistogram( &newData, "hPowerMag" );

        // The peak finding analysis uses TSpectrum
        // It is adapted from an example script written by Rene Brun: https://root.cern.ch/root/html/tutorials/spectrum/peaks.C.html
        // The search tolerance and threshold are configurable parameters

        TSpectrum* spectrum = new TSpectrum( 10 ); // Maximum number of peaks = 10
        int nfound = spectrum->Search( fitPoints, fTolerance, "", fThreshold );

        LINFO(evlog, "Found " << nfound << " candidate peaks to fit");

        // Estimate background using TSpectrum::Background
        TH1 *hb = spectrum->Background( fitPoints, 20, "same" );

        // Estimate linear background using a fitting method
        TF1 *fline = new TF1( "fline", "pol1", 0, 1e9 );
        fitPoints->Fit( "fline", "qn" );

        // Maximum 10 peaks = 32 parameters (3*10 + 2)
        double par[32];

        par[0] = fline->GetParameter( 0 );
        par[1] = fline->GetParameter( 1 );

        // Loop on all found peaks
        // Peaks with SNR < 2 will be discarded

        fNPeaks = 0;
        double *xpeaks = spectrum->GetPositionX();

        for( int peak = 0; peak < nfound; ++peak )
        {
            // Acquire peak location and bin
            double xp = xpeaks[peak];
            int bin = fitPoints->GetXaxis()->FindBin( xp );
            double yp = fitPoints->GetBinContent( bin );

            LINFO(evlog, "Looking at peak (" << xp << ", " << yp << ")");

            // If the y-position is less than twice the background level (SNR < 2), we will discard it
            if( yp < 2 * fline->Eval( xp ) )
            {
                LDEBUG(evlog, "Discarding peak");
                continue;
            }

            LDEBUG(evlog, "Keeping peak");

            // If it is kept, add the info to par
            par[3*fNPeaks + 2] = yp;
            par[3*fNPeaks + 3] = xp / 1e6;
            par[3*fNPeaks + 4] = 0.1; // Arbitrary default guess of 100kHz width

            // Increment npeaks
            ++fNPeaks;
        }

        LINFO(evlog, "Found " << fNPeaks << " useful peaks to fit");
        LINFO(evlog, "Now fitting: Be patient...");

        // Next we set up a TF1 to fit the power projection
        // Recall SumOfGaussians is a sum of npeaks Gaussians

        TF1 *fit = new TF1( "fit", SumOfGaussians, 0, 1e9, 32 );

        // We may have more than the default 25 parameters
        TVirtualFitter::Fitter( fitPoints, 10+3*fNPeaks );

        // Set guess parameters and limits
        // We will insist that all the Gaussian parameters are positive
        // The upper limits are arbitrary but much larger than anything sensible

        fit->SetParameters( par );
        for( int peak = 0; peak < fNPeaks; ++peak )
        {
            fit->SetParLimits( 3*peak+2, 0, 1000 );
            fit->SetParLimits( 3*peak+3, -50000, 50000 );
            fit->SetParLimits( 3*peak+4, 0, 100 );
        }
        fit->SetNpx( 1000 );

        // Fix all parameters which will not be used because npeaks < 10
        for( int tooManyPeaks = 9; tooManyPeaks >= fNPeaks; tooManyPeaks-- )
        {
            fit->FixParameter( 3*tooManyPeaks+2, 0 ); // norm = 0 should ensure no contribution
            fit->FixParameter( 3*tooManyPeaks+3, 0 );
            fit->FixParameter( 3*tooManyPeaks+4, 1 ); // just to avoid 0/0
        }

        // Perform fit
        TFitResultPtr fitStatus = fitPoints->Fit( "fit", "S" );

        LINFO(evlog, "Fit completed!");

        // Finally we put all of the fit information into the KTPowerFitData object
        // Vectors will hold the parameters and associated errors

        std::vector<double> norms, means, sigmas, maxima;
        std::vector<double> normErrs, meanErrs, sigmaErrs, maximumErrs;

        double invsqrt2pi = TMath::Power( 2*KTMath::Pi(), -0.5 ); // I could have just written the number but eh

        // Loop over found peaks and fill the vectors
        for( int peak = 0; peak < fNPeaks; ++peak )
        {
            norms.push_back( fit->GetParameter(3*peak+2) );
            means.push_back( fit->GetParameter(3*peak+3) );
            sigmas.push_back( fit->GetParameter(3*peak+4) );
            //maxima.push_back( invsqrt2pi * fit->GetParameter(3*p+2) / fit->GetParameter(3*p+4) );
            maxima.push_back( spectrum->GetPositionY()[peak] );

            normErrs.push_back( fit->GetParError(3*peak+2) );
            meanErrs.push_back( fit->GetParError(3*peak+3) );
            sigmaErrs.push_back( fit->GetParError(3*peak+4) );

            // Really annoying error propagation for this one
            maximumErrs.push_back( TMath::Sqrt( TMath::Power( invsqrt2pi / fit->GetParameter(3*peak+4) * fit->GetParError(3*peak+2), 2 ) + TMath::Power( invsqrt2pi * fit->GetParameter(3*peak+2) / TMath::Power( fit->GetParameter(3*peak+4), 2 ) * fit->GetParError(3*peak+4), 2 ) ) );
        }

        // We will also store the validity of the fit
        bool valid = fitStatus->IsValid();
        LINFO(evlog, "Fit validity = " << valid);

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

        // npeaks
        newData.SetNPeaks( fNPeaks );

        // Calculate first four moments from TH1 directly
        double meanCorrection = 0.5 * (minAlpha + maxAlpha);
        newData.SetAverage( (fitPoints->GetMean() - meanCorrection)/1e6 );
        newData.SetRMS( fitPoints->GetRMS()/1e6 );
        newData.SetSkewness( fitPoints->GetSkewness() );
        newData.SetKurtosis( fitPoints->GetKurtosis() );

        // Calculate classifiers
        LINFO(evlog, "Calculating classifiers for KTPowerFitData");

        // First find the most central peak
        int cpIndex;
        double cpLocation;
        for( int peak = 0; peak < fNPeaks; ++peak )
        {
            LDEBUG(evlog, "Peak " << peak << " has location " << xpeaks[peak] - meanCorrection);
            if( peak == 0 || std::abs( xpeaks[peak] - meanCorrection ) < std::abs( cpLocation ) )
            {
                cpIndex = peak;
                cpLocation = xpeaks[peak] - meanCorrection;
            }
        }

        LINFO(evlog, "Central peak has index " << cpIndex << " and location " << cpLocation);

        // Set central peak fit parameters
        newData.SetNormCentral( fit->GetParameter(3*cpIndex + 2) );
        newData.SetMeanCentral( fit->GetParameter(3*cpIndex + 3) - meanCorrection * 1.e-6 );
        newData.SetSigmaCentral( fit->GetParameter(3*cpIndex + 4) );
        newData.SetMaximumCentral( invsqrt2pi * fit->GetParameter(3*cpIndex + 2) / fit->GetParameter(3*cpIndex + 4) );

        // Vectors to calculate statistics near and away from the central peak
        std::vector<double> centralPoints;
        std::vector<double> nonCentralPoints;

        // Iterate over all points and fill the appropriate vector
        int iBin = 1;
        const KTPowerFitData::SetOfPoints& setOfPoints = newData.GetSetOfPoints();
        for( KTPowerFitData::SetOfPoints::const_iterator it = setOfPoints.begin(); it != setOfPoints.end(); ++it )
        {
            if( it->second.fAbscissa - meanCorrection >= newData.GetMeanCentral()*1e6 - 3 * newData.GetSigmaCentral()*1e6 && it->second.fAbscissa - meanCorrection <= newData.GetMeanCentral()*1e6 + 3 * newData.GetSigmaCentral()*1e6 )
            {
                centralPoints.push_back( it->second.fOrdinate );
                LDEBUG(evlog, "Added point at " << it->second.fAbscissa - meanCorrection << " to centralPoints vector");
            }
            else
            {
                nonCentralPoints.push_back( it->second.fOrdinate );
                LDEBUG(evlog, "Added point at " << it->second.fAbscissa - meanCorrection << " to nonCentralPoints vector");
            }
            ++iBin;
        }

        // Calculate central mean and RMS
        double centralMean = 0.0;
        double centralRMS = 0.0;
        for( int iPoint = 0; iPoint < centralPoints.size(); ++iPoint )
        {
            centralMean += centralPoints.at(iPoint);
            centralRMS += TMath::Power( centralPoints.at(iPoint), 2 );
        }
        double cpNorm = 1. / double(centralPoints.size());
        centralMean *= cpNorm;
        centralRMS = TMath::Power( centralRMS * cpNorm  - centralMean*centralMean, 0.5 );

        LINFO(evlog, "Calculated central mean = " << centralMean << " and RMS = " << centralRMS);

        // Calculate non-central mean and RMS
        double nonCentralMean = 0.0;
        double nonCentralRMS = 0.0;
        for( int iPoint = 0; iPoint < nonCentralPoints.size(); ++iPoint )
        {
            nonCentralMean += nonCentralPoints.at(iPoint);
            nonCentralRMS += nonCentralPoints.at(iPoint) * nonCentralPoints.at(iPoint);
        }
        double ncpNorm = 1. / double(nonCentralPoints.size());
        nonCentralMean *= ncpNorm;
        nonCentralRMS = TMath::Power( nonCentralRMS * ncpNorm - nonCentralMean*nonCentralMean, 0.5 );

        LINFO(evlog, "Calculated non-central mean = " << nonCentralMean << " and RMS = " << nonCentralRMS);

        // Fill data
        newData.SetRMSAwayFromCentral( nonCentralRMS );
        newData.SetCentralPowerFraction( centralMean / (centralMean + nonCentralMean) );

        // We copy the track intercept to newData
        newData.SetTrackIntercept( data.GetIntercept() );

        // Lastly we copy the track ID to newData
        newData.SetTrackID( data.GetTrackID() );

        LINFO(evlog, "Finished classifier calculations. Power fit data is done!");

        return true;
    }

    bool KTLinearDensityProbeFit::PerformTest(KTDiscriminatedPoints2DData& pts, KTLinearFitResult& newData, double fProbeWidth, double fStepSize, unsigned component)
    {
        double alpha = fMinFrequency;
        
        LINFO(evlog, "Performing density probe test with fProbeWidth = " << fProbeWidth << " and fStepSize = " << fStepSize);

        double bestAlpha = FindIntercept( pts, fStepSize, newData.GetSlope( component ), fProbeWidth );
        newData.SetIntercept( bestAlpha, component );

        return true;
    }

    void KTLinearDensityProbeFit::SlotFunctionThreshPoints( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTProcessedTrackData >())
        {
            LERROR(avlog_hh, "Data not found with type < KTProcessedTrackData >!");
            return;
        }
        if (! data->Has< KTDiscriminatedPoints2DData >())
        {
            LERROR(avlog_hh, "Data not found with type < KTDiscriminatedPoints2DData >!");
            return;
        }
        if (! data->Has< KTPSCollectionData >())
        {
            LERROR(avlog_hh, "Data not found with type < KTPSCollectionData >!");
            return;
        }

        // Call the function
        if( !ChooseAlgorithm( data->Of< KTProcessedTrackData >(), data->Of< KTDiscriminatedPoints2DData >(), data->Of< KTPSCollectionData >() ) )
        {
            LERROR(avlog_hh, "Density probe analysis failed.");
            return;
        }

        // Emit appropriate signal
        if( fDoDensityMaximization )
        {
            fLinearDensityFitSignal( data );
        }
        if( fDoProjectionAnalysis )
        {
            fPowerFitSignal( data );
        }

        return;
    }



} /* namespace Katydid */
