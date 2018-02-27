/*
 * TestGainVariationProcessor.cc
 *
 *  Created on: Feb 26, 2018
 *      Author: N.S. Oblath
 *
 *  Intended results
 *    From the ROOT file, you can make the following plots:
 *      - hFSAndVarFS + hFSOnlySpline + hFSAndVarSpline: The splines should sit right on top of the mean of hFSAndVarFS.
 *      - hFSAndVarVar + hFSOnlyVarianceSpline + hFSAndVarVarianceSpline: The splines should sit right on top of the mean of hFSAndVarVar.
 *        At the time of this writing, though, hFSAndVarVarSpline had a significant bias high.
 *    Note that hFSOnlyFS and hFSAndVarFS are explicitly exactly the same.
 */

#include "KT2ROOT.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumVarianceData.hh"
#include "KTFrequencySpectrumVariance.hh"
#include "KTGainVariationData.hh"
#include "KTGainVariationProcessor.hh"
#include "KTLogger.hh"
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

    KTFrequencySpectrumDataPolar fsDataOnly;
    KTFrequencySpectrumPolar* fsOnlyFS = new KTFrequencySpectrumPolar( nBins, minFreq, maxFreq );

    KTFrequencySpectrumDataPolar fsData;
    KTFrequencySpectrumPolar* fsAndVarFS = new KTFrequencySpectrumPolar( nBins, minFreq, maxFreq );

    KTFrequencySpectrumVarianceDataPolar& varData = fsData.Of< KTFrequencySpectrumVarianceDataPolar >();
    KTFrequencySpectrumVariance* fsAndVarVar = new KTFrequencySpectrumVariance( nBins, minFreq, maxFreq );

    for (unsigned iBin=0; iBin<nBins; iBin++)
    {
        noiseSigma = noiseSigmaLow + (double)(iBin) * noiseSigmaSlope;
        rand.param( KTRNGGaussian<>::param_type( meanValue, noiseSigma ) );
        value = rand();
        (*fsOnlyFS)(iBin).set_polar( value, 0. );
        (*fsAndVarFS)(iBin).set_polar( value, 0. );
        (*fsAndVarVar)(iBin) = noiseSigma * noiseSigma;
    }

    fsDataOnly.SetNComponents( 1 );
    fsDataOnly.SetSpectrum( fsOnlyFS, 0 );

    fsData.SetNComponents( 1 );
    fsData.SetSpectrum( fsAndVarFS, 0 );

    varData.SetNComponents( 1 );
    varData.SetSpectrum( fsAndVarVar, 0 );

    KTINFO(testlog, "Initializing gain variation");

    KTGainVariationProcessor gvProc;
    gvProc.SetMinFrequency( minFreq );
    gvProc.SetMaxFrequency( maxFreq);
    gvProc.SetNFitPoints( 10 );
    gvProc.SetNormalize( false );
    gvProc.SetVarianceCalcNBins(100);

    KTINFO(testlog, "Processing gain variation -- FS only");
    gvProc.CalculateGainVariation(fsDataOnly);

    KTINFO(testlog, "Processing gain variation -- FS & variance");
    gvProc.CalculateGainVariation(fsData, varData);

    KTGainVariationData& fsOnlyGVData = fsDataOnly.Of< KTGainVariationData >();
    KTGainVariationData& fsAndVarGVData = fsData.Of< KTGainVariationData >();

#ifdef ROOT_FOUND
    KTINFO(testlog, "Writing histograms to a ROOT file");

    TFile* file = new TFile("gain_var_proc_test.root", "recreate");

    TH1D* fsOnlyFSHist = KT2ROOT::CreateMagnitudeHistogram(fsOnlyFS, "hFSOnlyFS");
    fsOnlyFSHist->SetDirectory(file);
    fsOnlyFSHist->SetLineColor(kBlue);

    TH1D* fsAndVarFSHist = KT2ROOT::CreateMagnitudeHistogram(fsAndVarFS, "hFSAndVarFS");
    fsAndVarFSHist->SetDirectory(file);
    fsAndVarFSHist->SetLineColor(kBlue);

    TH1D* fsAndVarVarHist = KT2ROOT::CreateHistogram(fsAndVarVar, "hFSAndVarVar");
    fsAndVarVarHist->SetDirectory(file);
    fsAndVarVarHist->SetLineColor(kBlue);

    TH1D* fsOnlySplineHist = fsOnlyGVData.CreateGainVariationHistogram(nBins, 0, "hFSOnlySpline");
    fsOnlySplineHist->SetDirectory(file);
    fsOnlySplineHist->SetLineColor(kGreen+2);

    TH1D* fsOnlyVarSplineHist = fsOnlyGVData.CreateGainVariationVarianceHistogram(nBins, 0, "hFSOnlyVarSpline");
    fsOnlyVarSplineHist->SetDirectory(file);
    fsOnlyVarSplineHist->SetLineColor(kGreen+2);

    TH1D* fsAndVarSplineHist = fsAndVarGVData.CreateGainVariationHistogram(nBins, 0, "hFSAndVarSpline");
    fsAndVarSplineHist->SetDirectory(file);
    fsAndVarSplineHist->SetLineColor(kRed);

    TH1D* fsAndVarVarSplineHist = fsAndVarGVData.CreateGainVariationVarianceHistogram(nBins, 0, "hFSAndVarVarSpline");
    fsAndVarVarSplineHist->SetDirectory(file);
    fsAndVarVarSplineHist->SetLineColor(kRed);

    file->Write();
    file->Close();
    delete file;
#endif

    return 0;
}

