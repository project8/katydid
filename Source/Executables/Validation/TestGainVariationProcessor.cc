/*
 * TestGainVariationProcessor.cc
 *
 *  Created on: Feb 26, 2018
 *      Author: N.S. Oblath
 *
 *  Intended results
 *    From the ROOT file, you can make the following plots:
 *      - hPSAndVarPS + hPSOnlySpline + hPSAndVarSpline: The splines should sit right on top of the mean of hPSAndVarPS.
 *      - hPSAndVarVar + hPSOnlyVarianceSpline + hPSAndVarVarianceSpline: The splines should sit right on top of the mean of hPSAndVarVar.
 *        At the time of this writing, though, hPSAndVarVarSpline had a significant bias high.
 *    Note that hPSOnlyPS and hPSAndVarPS are explicitly exactly the same.
 */

#include "KT2ROOT.hh"
#include "KTFrequencySpectrumVarianceData.hh"
#include "KTFrequencySpectrumVariance.hh"
#include "KTGainVariationData.hh"
#include "KTGainVariationProcessor.hh"
#include "KTLogger.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTRandom.hh"

#include "TFile.h"


using namespace Katydid;

KTLOGGER(testlog, "TestGainVariationProcessor");

int main()
{

    unsigned nBins = 8192;
    double minFreq = 0., maxFreq = 100.;

    double meanValue = 10.;
    double noiseSigmaLow = 5.;
    double noiseSigmaHigh = 15.;
    double noiseSigma = 0.;

    double noiseSigmaSlope = (noiseSigmaHigh - noiseSigmaLow) / (double)nBins;

    double value = 0.;

    KTRNGEngine* engine = KTGlobalRNGEngine::get_instance();
    engine->SetSeed(20398);
    KTRNGGaussian<> rand( meanValue, noiseSigma );

    KTINFO(testlog, "Setting up input data");

    KTPowerSpectrumData psDataOnly;
    KTPowerSpectrum* psOnlyPS = new KTPowerSpectrum( nBins, minFreq, maxFreq );

    KTPowerSpectrumData psData;
    KTPowerSpectrum* psAndVarPS = new KTPowerSpectrum( nBins, minFreq, maxFreq );

    KTPowerSpectrumVarianceData& varData = psData.Of< KTPowerSpectrumVarianceData >();
    KTFrequencySpectrumVariance* psAndVarVar = new KTFrequencySpectrumVariance( nBins, minFreq, maxFreq );

    for (unsigned iBin=0; iBin<nBins; iBin++)
    {
        noiseSigma = noiseSigmaLow + (double)(iBin) * noiseSigmaSlope;
        rand.param( KTRNGGaussian<>::param_type( meanValue, noiseSigma ) );
        value = rand();
        (*psOnlyPS)(iBin) = value;
        (*psAndVarPS)(iBin) = value;
        (*psAndVarVar)(iBin) = noiseSigma * noiseSigma;
    }

    psDataOnly.SetNComponents( 1 );
    psDataOnly.SetSpectrum( psOnlyPS, 0 );

    psData.SetNComponents( 1 );
    psData.SetSpectrum( psAndVarPS, 0 );

    varData.SetNComponents( 1 );
    varData.SetSpectrum( psAndVarVar, 0 );

    KTINFO(testlog, "Initializing gain variation");

    KTGainVariationProcessor gvProc;
    gvProc.SetMinFrequency( minFreq );
    gvProc.SetMaxFrequency( maxFreq);
    gvProc.SetNFitPoints( 10 );
    gvProc.SetNormalize( false );
    gvProc.SetVarianceCalcNBins(100);

    KTINFO(testlog, "Processing gain variation -- PS only");
    gvProc.CalculateGainVariation(psDataOnly);

    KTINFO(testlog, "Processing gain variation -- PS & variance");
    gvProc.CalculateGainVariation(psData, varData);

    KTGainVariationData& psOnlyGVData = psDataOnly.Of< KTGainVariationData >();
    KTGainVariationData& psAndVarGVData = psData.Of< KTGainVariationData >();

#ifdef ROOT_FOUND
    KTINFO(testlog, "Writing histograms to a ROOT file");

    TFile* file = new TFile("gain_var_proc_test.root", "recreate");

    TH1D* psOnlyPSHist = KT2ROOT::CreatePowerHistogram(psOnlyPS, "hPSOnlyPS");
    psOnlyPSHist->SetDirectory(file);
    psOnlyPSHist->SetLineColor(kBlue);

    TH1D* psAndVarPSHist = KT2ROOT::CreatePowerHistogram(psAndVarPS, "hPSAndVarPS");
    psAndVarPSHist->SetDirectory(file);
    psAndVarPSHist->SetLineColor(kBlue);

    TH1D* psAndVarVarHist = KT2ROOT::CreateHistogram(psAndVarVar, "hPSAndVarVar");
    psAndVarVarHist->SetDirectory(file);
    psAndVarVarHist->SetLineColor(kBlue);

    TH1D* psOnlySplineHist = psOnlyGVData.CreateGainVariationHistogram(nBins, 0, "hPSOnlySpline");
    psOnlySplineHist->SetDirectory(file);
    psOnlySplineHist->SetLineColor(kGreen+2);

    TH1D* psOnlyVarSplineHist = psOnlyGVData.CreateGainVariationVarianceHistogram(nBins, 0, "hPSOnlyVarSpline");
    psOnlyVarSplineHist->SetDirectory(file);
    psOnlyVarSplineHist->SetLineColor(kGreen+2);

    TH1D* psAndVarSplineHist = psAndVarGVData.CreateGainVariationHistogram(nBins, 0, "hPSAndVarSpline");
    psAndVarSplineHist->SetDirectory(file);
    psAndVarSplineHist->SetLineColor(kRed);

    TH1D* psAndVarVarSplineHist = psAndVarGVData.CreateGainVariationVarianceHistogram(nBins, 0, "hPSAndVarVarSpline");
    psAndVarVarSplineHist->SetDirectory(file);
    psAndVarVarSplineHist->SetLineColor(kRed);

    file->Write();
    file->Close();
    delete file;
#endif

    return 0;
}

