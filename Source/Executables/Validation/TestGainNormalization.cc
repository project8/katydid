/*
 * TestGainNormalization.cc
 *
 *  Created on: Feb 27, 2018
 *      Author: N.S. Oblath
 *
 *  Intended results
 *    From the ROOT file, you can make the following plots:
 *      - hPS + hNormPS: hNormPS will have a normalized mean and variance relative to hPS.
 *    Note that hPSOnlyPS and hPSAndVarPS are explicitly exactly the same.
 */

#include "KT2ROOT.hh"
#include "KTFrequencySpectrumVarianceData.hh"
#include "KTFrequencySpectrumVariance.hh"
#include "KTGainNormalization.hh"
#include "KTGainVariationData.hh"
#include "KTGainVariationProcessor.hh"
#include "KTLogger.hh"
#include "KTNormalizedFSData.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTRandom.hh"

#include "TFile.h"


using namespace Katydid;

KTLOGGER(testlog, "TestGainNormalization");

int main()
{

    unsigned nBins = 8192;
    double minFreq = 0., maxFreq = 100.;

    double meanValueLow = 8.;
    double meanValueHigh = 32.;
    double noiseSigmaLow = 5.;
    double noiseSigmaHigh = 15.;

    double meanValueSlope = (meanValueLow - meanValueHigh) / (double)nBins; // downwards sloping
    double noiseSigmaSlope = (noiseSigmaHigh - noiseSigmaLow) / (double)nBins; // growing

    double value = 0.;

    KTRNGEngine* engine = KTGlobalRNGEngine::get_instance();
    engine->SetSeed(20398);
    KTRNGGaussian<> rand;

    KTINFO(testlog, "Setting up input data");

    KTPowerSpectrumData psData;
    KTPowerSpectrum* ps = new KTPowerSpectrum( nBins, minFreq, maxFreq );

    KTPowerSpectrumVarianceData& varData = psData.Of< KTPowerSpectrumVarianceData >();
    KTFrequencySpectrumVariance* var = new KTFrequencySpectrumVariance( nBins, minFreq, maxFreq );

    double meanValue = 0.;
    double noiseSigma = 0.;
    for (unsigned iBin=0; iBin<nBins; iBin++)
    {
        meanValue = meanValueLow + (double)iBin * meanValueSlope;
        noiseSigma = noiseSigmaLow + (double)iBin * noiseSigmaSlope;
        rand.param( KTRNGGaussian<>::param_type( meanValue, noiseSigma ) );
        value = rand();
        (*ps)(iBin) = value;
        (*var)(iBin) = noiseSigma * noiseSigma;
    }

    psData.SetNComponents( 1 );
    psData.SetSpectrum( ps, 0 );

    varData.SetNComponents( 1 );
    varData.SetSpectrum( var, 0 );

    KTINFO(testlog, "Initializing gain variation");

    KTGainVariationProcessor gvProc;
    gvProc.SetMinFrequency( minFreq );
    gvProc.SetMaxFrequency( maxFreq);
    gvProc.SetNFitPoints( 10 );
    gvProc.SetNormalize( false );
    gvProc.SetVarianceCalcNBins(100);

    KTINFO(testlog, "Processing gain variation");
    gvProc.CalculateGainVariation(psData, varData);

    KTGainVariationData& gvData = psData.Of< KTGainVariationData >();

    KTINFO(testlog, "Initializing gain normalization");

    KTGainNormalization gainNorm;
    gainNorm.SetMinFrequency( minFreq );
    gainNorm.SetMaxFrequency( maxFreq );

    KTINFO(testlog, "Processing normalization");

    gainNorm.Normalize(psData, gvData);

    KTNormalizedPSData& normPSData = psData.Of< KTNormalizedPSData >();
    KTPowerSpectrum* normPS = normPSData.GetSpectrum(0);

#ifdef ROOT_FOUND
    KTINFO(testlog, "Writing histograms to a ROOT file");

    TFile* file = new TFile("gain_norm_test.root", "recreate");

    TH1D* psHist = KT2ROOT::CreatePowerHistogram(ps, "hPS");
    psHist->SetDirectory(file);
    psHist->SetLineColor(kBlue-4);

    TH1D* varHist = KT2ROOT::CreateHistogram(var, "hVar");
    varHist->SetDirectory(file);
    varHist->SetLineColor(kBlue-4);

    TH1D* psSplineHist = gvData.CreateGainVariationHistogram(nBins, 0, "hPSSpline");
    psSplineHist->SetDirectory(file);
    psSplineHist->SetLineColor(kRed);

    TH1D* varSplineHist = gvData.CreateGainVariationVarianceHistogram(nBins, 0, "hVarSpline");
    varSplineHist->SetDirectory(file);
    varSplineHist->SetLineColor(kRed);

    TH1D* normPSHist = KT2ROOT::CreatePowerHistogram(normPS, "hNormPS");
    normPSHist->SetDirectory(file);
    normPSHist->SetLineColor(kGreen+2);

    file->Write();
    file->Close();
    delete file;
#endif

    return 0;
}

