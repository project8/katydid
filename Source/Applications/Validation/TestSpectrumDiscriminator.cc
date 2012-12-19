/*
 * TestSpectrumDiscriminator.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTDiscriminatedPoints1DData.hh"
#include "KTFrequencySpectrum.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTLogger.hh"
#include "KTSpectrumDiscriminator.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH1.h"
#include "TRandom3.h"
#endif

using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "katydid.applications.validation");

int main()
{
    UInt_t nBins = 1000000;
    Double_t minFreq = 0., maxFreq = 100.;

    Double_t meanValue = 5.;
    Double_t noiseSigma = 1.;

    UInt_t nPeaks = 10;
    Double_t meanPeakMult = 5.;
    Double_t sigmaPeakMult = 3.;

    Double_t sigmaThresh = 5.;

#ifdef ROOT_FOUND
    TRandom3 rand(0);
#endif

    KTFrequencySpectrumData data(1);
    KTFrequencySpectrum* spectrum = new KTFrequencySpectrum(nBins, minFreq, maxFreq);

    // Fill in the noise
    KTINFO(testlog, "Creating the baseline and noise");
    for (UInt_t iBin=0; iBin<nBins; iBin++)
    {
#ifdef ROOT_FOUND
        (*spectrum)(iBin).set_polar(rand.Gaus(meanValue, noiseSigma), 0.);
#else
        (*spectrum)(iBin).set_polar(meanValue, 0.);
#endif
    }

    // Add some peaks
    for (UInt_t iPeak=0; iPeak<nPeaks; iPeak++)
    {
        UInt_t iBin = UInt_t(rand.Rndm() * (Double_t)nBins);
        Double_t multiplier = rand.Gaus(meanPeakMult, sigmaPeakMult);
        (*spectrum)(iBin).set_polar((*spectrum)(iBin).abs() * multiplier, 0.);
        KTINFO(testlog, "Adding peak at bin " << iBin << "; new value: " << (*spectrum)(iBin).abs());
    }

    data.SetSpectrum(spectrum, 0);

#ifdef ROOT_FOUND
    TFile* file = new TFile("spectrum_disc_test.root", "recreate");

    TH1D* histFreqSpec = spectrum->CreateMagnitudeHistogram("hFreqSpectrum");
    histFreqSpec->SetDirectory(file);
    histFreqSpec->Write();
#endif


    KTSpectrumDiscriminator disc;
    disc.SetMinFrequency(minFreq);
    disc.SetMaxFrequency(maxFreq);
    disc.SetSigmaThreshold(sigmaThresh);

    KTINFO(testlog, "Discriminating data");
    KTDiscriminatedPoints1DData* pointData = disc.Discriminate(&data);

    KTDiscriminatedPoints1DData::SetOfPoints setOfPoints = pointData->GetSetOfPoints(0);
    KTINFO(testlog, "Found " << setOfPoints.size() << " points above threshold");
    for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it=setOfPoints.begin(); it != setOfPoints.end(); it++)
    {
        KTINFO(testlog, "Bin " << it->first << " = " << it->second);
    }

#ifdef ROOT_FOUND
    TH1D* histPoints = (TH1D*)histFreqSpec->Clone();
    histPoints->SetName("hPoints");
    histPoints->Reset();
    histPoints->SetLineColor(2);
    for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it=setOfPoints.begin(); it != setOfPoints.end(); it++)
    {
        histPoints->SetBinContent(it->first + 1, it->second);
    }
    histPoints->Write();
#endif

    delete pointData;

#ifdef ROOT_FOUND
    file->Close();
    delete file;
#endif

    return 0;
}
