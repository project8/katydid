/*
 * TestGainVariation.cc
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 *
 *      Usage:
 *      > TestGainVariation [nBins [nFitPoints [true_A true_B true_C [noise_sigma]]]]
 */

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

int main(int argc, char** argv)
{
    KTINFO(vallog, "Commencing gain variation test");

    UInt_t nBins = 1000000;

    UInt_t nFitPoints = 3;

    KTGainVariationProcessor::FitResult trueValues;
    trueValues.fA = 0.;
    trueValues.fB = 0.;
    trueValues.fC = 15.;

    Double_t noiseSigma = 20.;

    if (argc >= 2)
    {
        nBins = atoi(argv[1]);

        if (argc >= 3)
        {
            nFitPoints = atoi(argv[2]);

            if (argc >= 6)
            {
                trueValues.fA = atof(argv[3]);
                trueValues.fB = atof(argv[4]);
                trueValues.fC = atof(argv[5]);

                if (argc >= 7)
                {
                    noiseSigma = atof(argv[6]);
                }
            }
        }
    }

    KTINFO(vallog, "Number of fit points: " << nFitPoints);
    KTINFO(vallog, "True function parameters:\n" <<
           "\tA: " << trueValues.fA << '\n' <<
           "\tB: " << trueValues.fB << '\n' <<
           "\tC: " << trueValues.fC << '\n');

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

    for (UInt_t iBin=0; iBin < nBins; iBin++)
    {
#ifdef ROOT_FOUND
        (*spectrum)(iBin) = rand.Gaus(gainVarProc.FitFunction(trueValues, spectrum->GetBinCenter(iBin)), noiseSigma);
#else
        (*spectrum)(iBin) = gainVarProc.FitFunction(trueValues, spectrum->GetBinCenter(iBin));
#endif
    }

    fsData.SetSpectrum(spectrum);

#ifdef ROOT_FOUND
    TFile* file = new TFile("gain_var_test.root", "recreate");

    TH1D* spectrumHist = spectrum->CreateMagnitudeHistogram("hInputMag");
    spectrumHist->Write();
#endif

    KTINFO(vallog, "Performing fit");

    KTGainVariationData* gvData = gainVarProc.PerformFit(&fsData);

    KTGainVariationProcessor::FitResult fitResult = gvData->GetFitResult(0);
    KTINFO(vallog, "Fit function parameters:\n" <<
           "\tA: " << fitResult.fA << '\n' <<
           "\tB: " << fitResult.fB << '\n' <<
           "\tC: " << fitResult.fC << '\n');


#ifdef ROOT_FOUND
    TH1D* fitHist = gvData->CreateGainVariationHistogram(0);
    fitHist->SetLineColor(8);
    fitHist->Write();

    TH1D* flatSpectrumHist = (TH1D*)spectrumHist->Clone();
    flatSpectrumHist->SetName("hOutputMag");
    flatSpectrumHist->Divide(fitHist);
    flatSpectrumHist->SetLineColor(2);
    flatSpectrumHist->Write();
#endif

    delete gvData;

#ifdef ROOT_FOUND
    file->Close();
    delete file;
#endif

    return 0;
}
