/*
 * TestGainVariation.cc
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 *
 *      Test the gain variation processor against a parabola
 *
 *      Usage:
 *      > TestGainVariation [nBins [nFitPoints [true_A true_B true_C [noise_sigma]]]]
 */

#include "KTGainNormalization.hh"
#include "KTGainVariationData.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTLogger.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH1.h"

#include "TRandom3.h"
#endif

#include <cstdlib>

using namespace Katydid;
using namespace std;

KTLOGGER(vallog, "katydid.applications.validation");

struct Parameters
{
    // y = fA * x^2 + fB * x + fC
    Double_t fA;
    Double_t fB;
    Double_t fC;
};

Double_t TestFunction(const Parameters& results, Double_t x);


int main(int argc, char** argv)
{
    KTINFO(vallog, "Commencing gain variation test");

    UInt_t nBins = 1000000;

    UInt_t nFitPoints = 3;

    Parameters funcParams;
    funcParams.fA = 0.;
    funcParams.fB = 0.;
    funcParams.fC = 15.;

    Double_t noiseSigma = 20.;

    if (argc >= 2)
    {
        nBins = atoi(argv[1]);

        if (argc >= 3)
        {
            nFitPoints = atoi(argv[2]);

            if (argc >= 6)
            {
                funcParams.fA = atof(argv[3]);
                funcParams.fB = atof(argv[4]);
                funcParams.fC = atof(argv[5]);

                if (argc >= 7)
                {
                    noiseSigma = atof(argv[6]);
                }
            }
        }
    }

    KTINFO(vallog, "Number of fit points: " << nFitPoints);
    KTINFO(vallog, "Function parameters:\n" <<
           "\tA: " << funcParams.fA << '\n' <<
           "\tB: " << funcParams.fB << '\n' <<
           "\tC: " << funcParams.fC << '\n');

    KTGainVariationProcessor gainVarProc;
    gainVarProc.SetMinBin(0);
    gainVarProc.SetMaxBin(nBins-1);
    gainVarProc.SetNFitPoints(nFitPoints);

    KTFrequencySpectrumData fsData(1);
    KTFrequencySpectrum* spectrum = new KTFrequencySpectrum(nBins, 0., 100.);

#ifdef ROOT_FOUND
    TRandom3 rand(0);
    KTINFO(vallog, "Adding Gaussian noise with sigma = " << noiseSigma);
#else
    KTWARN(vallog, "No noise is being added");
#endif

    Double_t value;
    Double_t value0 = TestFunction(funcParams, spectrum->GetBinCenter(0));
    for (UInt_t iBin=0; iBin < nBins; iBin++)
    {
#ifdef ROOT_FOUND
        value = TestFunction(funcParams, spectrum->GetBinCenter(iBin));
        (*spectrum)(iBin).set_polar(rand.Gaus(value, value * noiseSigma / value0), 0.);
        if (iBin < 100)
        {
            KTDEBUG(vallog, "Point " << iBin << "   " << (*spectrum)(iBin));
        }
#else
        (*spectrum)(iBin).set_polar(TestFunction(funcParams, spectrum->GetBinCenter(iBin)));
#endif
    }

    fsData.SetSpectrum(spectrum);

#ifdef ROOT_FOUND
    TFile* file = new TFile("gain_var_test.root", "recreate");

    TH1D* spectrumHist = spectrum->CreateMagnitudeHistogram("hInputMag");
    spectrumHist->Write();
#endif

    KTINFO(vallog, "Performing variation calculation");

    KTGainVariationData* gvData = gainVarProc.CalculateGainVariation(&fsData);

    /*
    KTGainVariationProcessor::FitResult fitResult = gvData->GetFitResult(0);
    KTINFO(vallog, "Fit function parameters:\n" <<
           "\tA: " << fitResult.fA << '\n' <<
           "\tB: " << fitResult.fB << '\n' <<
           "\tC: " << fitResult.fC << '\n');
     */

#ifdef ROOT_FOUND
    TH1D* fitHist = gvData->CreateGainVariationHistogram(100, 0, "hGainVar");
    fitHist->SetLineColor(8);
    fitHist->Write();
/*
    TH1D* flatSpectrumHist = (TH1D*)spectrumHist->Clone();
    flatSpectrumHist->SetName("hOutputMag");
    flatSpectrumHist->Divide(fitHist);
    flatSpectrumHist->SetLineColor(2);
    flatSpectrumHist->Write();
    */
#endif

    KTGainNormalization gainNorm;
    gainNorm.SetMinBin(0);
    gainNorm.SetMaxBin(nBins-1);

    KTINFO(vallog, "Normalizing the spectrum");
    KTFrequencySpectrumData* normData = gainNorm.Normalize(&fsData, gvData);

    KTINFO(vallog, "Processing complete");
#ifdef ROOT_FOUND
    TH1D* normalizedHist = normData->GetSpectrum(0)->CreateMagnitudeHistogram("hOutputMag");
    normalizedHist->SetLineColor(2);
    normalizedHist->Write();
#endif

    delete gvData;
    delete normData;

#ifdef ROOT_FOUND
    file->Close();
    delete file;
#endif

    return 0;
}


Double_t TestFunction(const Parameters& params, Double_t x)
{
    return params.fA * x * x + params.fB * x + params.fC;
}

