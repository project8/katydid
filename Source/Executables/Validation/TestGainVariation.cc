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

#include "KT2ROOT.hh"
#include "KTGainNormalization.hh"
#include "KTGainVariationData.hh"
#include "KTGainVariationProcessor.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "logger.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH1.h"

#include "TRandom3.h"
#endif

#include <cstdlib>

using namespace Katydid;
using namespace std;

LOGGER(vallog, "TestGainVariation");

struct Parameters
{
    // y = fA * x^2 + fB * x + fC
    double fA;
    double fB;
    double fC;
};

double TestFunction(const Parameters& results, double x);


int main(int argc, char** argv)
{
    LINFO(vallog, "Commencing gain variation test");

    unsigned nBins = 1000000;

    unsigned nFitPoints = 3;

    Parameters funcParams;
    funcParams.fA = 0.;
    funcParams.fB = 0.;
    funcParams.fC = 15.;

    double noiseSigma = 20.;

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

    LINFO(vallog, "Number of fit points: " << nFitPoints);
    LINFO(vallog, "Function parameters:\n" <<
           "\tA: " << funcParams.fA << '\n' <<
           "\tB: " << funcParams.fB << '\n' <<
           "\tC: " << funcParams.fC << '\n');

    KTGainVariationProcessor gainVarProc;
    gainVarProc.SetMinBin(0);
    gainVarProc.SetMaxBin(nBins-1);
    gainVarProc.SetNFitPoints(nFitPoints);

    KTFrequencySpectrumDataPolar fsData;
    fsData.SetNComponents(1);
    KTFrequencySpectrumPolar* spectrum = new KTFrequencySpectrumPolar(nBins, 0., 100.);

#ifdef ROOT_FOUND
    TRandom3 rand(0);
    LINFO(vallog, "Adding Gaussian noise with sigma = " << noiseSigma);
#else
    LWARN(vallog, "No noise is being added");
#endif

    double value;
    double value0 = TestFunction(funcParams, spectrum->GetBinCenter(0));
    for (unsigned iBin=0; iBin < nBins; iBin++)
    {
#ifdef ROOT_FOUND
        value = TestFunction(funcParams, spectrum->GetBinCenter(iBin));
        (*spectrum)(iBin).set_polar(rand.Gaus(value, value * noiseSigma / value0), 0.);
        if (iBin < 100)
        {
            LDEBUG(vallog, "Point " << iBin << "   " << (*spectrum)(iBin));
        }
#else
        (*spectrum)(iBin).set_polar(TestFunction(funcParams, spectrum->GetBinCenter(iBin)));
#endif
    }

    fsData.SetSpectrum(spectrum);

#ifdef ROOT_FOUND
    TFile* file = new TFile("gain_var_test.root", "recreate");

    TH1D* spectrumHist = KT2ROOT::CreateMagnitudeHistogram(spectrum, "hInputMag");
    spectrumHist->Write();
#endif

    LINFO(vallog, "Performing variation calculation");

    if (! gainVarProc.CalculateGainVariation(fsData))
    {
        LERROR(vallog, "Gain variation failed");
        return -1;
    }
    KTGainVariationData& gvData = fsData.Of< KTGainVariationData >();

    /*
    KTGainVariationProcessor::FitResult fitResult = gvData->GetFitResult(0);
    LINFO(vallog, "Fit function parameters:\n" <<
           "\tA: " << fitResult.fA << '\n' <<
           "\tB: " << fitResult.fB << '\n' <<
           "\tC: " << fitResult.fC << '\n');
     */

#ifdef ROOT_FOUND
    TH1D* fitHist = gvData.CreateGainVariationHistogram(100, 0, "hGainVar");
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

    LINFO(vallog, "Normalizing the spectrum");
    if (! gainNorm.Normalize(fsData, gvData))
    {
        LERROR(vallog, "Somethign went wrong during gain normalization");
        return -1;
    }
    KTFrequencySpectrumDataPolar& normData = fsData.Of< KTFrequencySpectrumDataPolar >();

    LINFO(vallog, "Processing complete");
#ifdef ROOT_FOUND
    TH1D* normalizedHist = KT2ROOT::CreateMagnitudeHistogram(normData.GetSpectrumPolar(0), "hOutputMag");
    normalizedHist->SetLineColor(2);
    normalizedHist->Write();
#endif

#ifdef ROOT_FOUND
    file->Close();
    delete file;
#endif

    return 0;
}


double TestFunction(const Parameters& params, double x)
{
    return params.fA * x * x + params.fB * x + params.fC;
}

