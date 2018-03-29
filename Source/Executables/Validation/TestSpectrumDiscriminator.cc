/*
 * TestSpectrumDiscriminator.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KT2ROOT.hh"
#include "KTDiscriminatedPoints1DData.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTLogger.hh"
#include "KTSpectrumDiscriminator.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH1.h"
#include "TRandom3.h"
#endif

using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "TestSpectrumDiscriminator");

int main()
{
    unsigned nBins = 1000;
    double minFreq = 0., maxFreq = 100.;

    double meanValue = 5.;
    double noiseSigma = 1.;

    unsigned nPeaks = 10;
    double meanPeakMult = 5.;
    double sigmaPeakMult = 3.;

    double sigmaThresh = 5.;

#ifdef ROOT_FOUND
    TRandom3 rand(0);
#endif

    KTFrequencySpectrumDataPolar data;
    data.SetNComponents(1);
    KTFrequencySpectrumPolar* spectrum = new KTFrequencySpectrumPolar(nBins, minFreq, maxFreq);

    // Fill in the noise
    KTINFO(testlog, "Creating the baseline and noise");
    for (unsigned iBin=0; iBin<nBins; iBin++)
    {
#ifdef ROOT_FOUND
        (*spectrum)(iBin).set_polar(rand.Gaus(meanValue, noiseSigma), 0.);
#else
        (*spectrum)(iBin).set_polar(meanValue, 0.);
#endif
    }

    // Add some peaks
    for (unsigned iPeak=0; iPeak<nPeaks; iPeak++)
    {
#ifdef ROOT_FOUND
        unsigned iBin = unsigned(rand.Rndm() * (double)nBins);
        double multiplier = rand.Gaus(meanPeakMult, sigmaPeakMult);
#else
        unsigned iBin = iPeak * nBins/nPeaks;
        double multiplier = meanPeakMult;
#endif
        (*spectrum)(iBin).set_polar((*spectrum)(iBin).abs() * multiplier, 0.);
        KTINFO(testlog, "Adding peak at bin " << iBin << "; new value: " << (*spectrum)(iBin).abs());
    }

    data.SetSpectrum(spectrum, 0);

#ifdef ROOT_FOUND
    TFile* file = new TFile("spectrum_disc_test.root", "recreate");

    TH1D* histFreqSpec = KT2ROOT::CreateMagnitudeHistogram(spectrum, "hFreqSpectrum");
    histFreqSpec->SetDirectory(file);
    histFreqSpec->Write();
#endif


    KTSpectrumDiscriminator disc;
    disc.SetMinFrequency(minFreq);
    disc.SetMaxFrequency(maxFreq);
    disc.SetSigmaThreshold(sigmaThresh);

    KTINFO(testlog, "Discriminating data");
    if (! disc.Discriminate(data))
    {
        KTERROR(testlog, "Something went wrong while discriminating peaks");
        return -1;
    }
    KTDiscriminatedPoints1DData& pointData = data.Of< KTDiscriminatedPoints1DData >();

    KTDiscriminatedPoints1DData::SetOfPoints setOfPoints = pointData.GetSetOfPoints(0);
    KTINFO(testlog, "Found " << setOfPoints.size() << " points above threshold");
    for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it=setOfPoints.begin(); it != setOfPoints.end(); it++)
    {
        KTINFO(testlog, "Bin " << it->first << " = (" << it->second.fAbscissa << ", " << it->second.fOrdinate << ")");
    }

#ifdef ROOT_FOUND
    TH1D* histPoints = (TH1D*)histFreqSpec->Clone();
    histPoints->SetName("hPoints");
    histPoints->Reset();
    histPoints->SetLineColor(2);
    for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it=setOfPoints.begin(); it != setOfPoints.end(); it++)
    {
        histPoints->SetBinContent(it->first + 1, it->second.fOrdinate);
    }
    histPoints->Write();
#endif

#ifdef ROOT_FOUND
    file->Close();
    delete file;
#endif

    return 0;
}
