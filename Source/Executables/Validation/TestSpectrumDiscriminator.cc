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
#include "KTFrequencySpectrumFFTW.hh"
#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTPowerSpectrum.hh"
#include "KTPowerSpectrumData.hh"
#include "KTLogger.hh"
#include "KTSpectrumDiscriminator.hh"
#include "KTVariableSpectrumDiscriminator.hh"
#include "KTGainVariationData.hh"

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

    double meanValue = 10.;
    double noiseSigma = 1;

    unsigned nPeaks = 10;
    double meanPeakMult = 10.;

#ifdef ROOT_FOUND
    double sigmaPeakMult = 1.;
#endif
    
    //double sigmaThresh = 5.;
    double snrThresh = 5.;

#ifdef ROOT_FOUND
    TRandom3 rand(0);
#endif


    // Create gain variation data
    unsigned NFitPoints = 20;
    double* xVals = new double[NFitPoints];
    double* yValsMean = new double[NFitPoints];
    double* yValsVar = new double[NFitPoints];

    unsigned nBinsPerFitPoint = nBins / NFitPoints;
    double dF = (maxFreq - minFreq) / nBinsPerFitPoint;

    for (unsigned iFitPoint=0; iFitPoint < NFitPoints; ++iFitPoint)
    {
        //unsigned fitPointStartBin = iFitPoint * nBinsPerFitPoint;

        xVals[iFitPoint] = minFreq + (iFitPoint + 0.5) * dF;
        yValsMean[iFitPoint] = meanValue;
        yValsVar[iFitPoint] = noiseSigma * noiseSigma;
    }
    yValsMean[(unsigned) (NFitPoints/2)] = meanValue /2.;

    KTSpline* splineMean = new KTSpline(xVals, yValsMean, NFitPoints);
    splineMean->SetXMin(minFreq);
    splineMean->SetXMax(maxFreq);

    KTSpline* splineVar = new KTSpline(xVals, yValsVar, NFitPoints);
    splineVar->SetXMin(minFreq);
    splineVar->SetXMax(maxFreq);

    delete [] xVals;
    delete [] yValsMean;
    delete [] yValsVar;

    KTGainVariationData gvdata;
    gvdata.SetNComponents(1);
    gvdata.SetSpline(splineMean);
    gvdata.SetVarianceSpline(splineVar, 0);

    // Check things have arrived
    std::shared_ptr< KTSpline::Implementation > splineImp = gvdata.GetSpline(0)->Implement(nBins, minFreq, maxFreq);
    double returnedMean = splineImp->GetMean();
    KTINFO(testlog, "Spline mean: "<<returnedMean);




    // Create frequency spectrum
    KTFrequencySpectrumDataPolar data;
    data.SetNComponents(1);
    KTFrequencySpectrumPolar* spectrum = new KTFrequencySpectrumPolar(nBins, minFreq, maxFreq);

    KTFrequencySpectrumDataFFTW dataFFTW;
    dataFFTW.SetNComponents(1);
    KTFrequencySpectrumFFTW* spectrumFFTW = new KTFrequencySpectrumFFTW(nBins, minFreq, maxFreq);

    KTPowerSpectrumData dataPS;
    dataPS.SetNComponents(1);
    KTPowerSpectrum* powerSpectrum = new KTPowerSpectrum(nBins, minFreq, maxFreq);

    // Fill in the noise
    KTINFO(testlog, "Creating the baseline and noise");
    for (unsigned iBin=0; iBin<nBins; iBin++)
    {
#ifdef ROOT_FOUND
        (*spectrum)(iBin).set_polar(rand.Gaus(meanValue, noiseSigma), 0.);
        (*spectrumFFTW)(iBin)[0] = rand.Gaus(meanValue, noiseSigma);
        (*spectrumFFTW)(iBin)[1] = 0.;
        (*powerSpectrum)(iBin) = rand.Gaus(meanValue, noiseSigma);
#else
        (*spectrum)(iBin).set_polar(meanValue, 0.);
        (*spectrumFFTW)(iBin)[0] = meanValue;
        (*spectrumFFTW)(iBin)[1] = 0.;
        (*powerSpectrum)(iBin) = meanValue;
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
        (*spectrumFFTW)(iBin)[0] =(*spectrumFFTW)(iBin)[0] * multiplier;
        (*spectrumFFTW)(iBin)[1] =(*spectrumFFTW)(iBin)[1] * multiplier;
        (*powerSpectrum)(iBin) = (*powerSpectrum)(iBin) * multiplier;
        //KTINFO(testlog, "Adding peak at bin " << iBin << "; new value: " << (*spectrum)(iBin).abs());
        //KTINFO(testlog, "Adding peak at bin " << iBin << "; new value: " << std::sqrt((*spectrumFFTW)(iBin)[0] * (*spectrumFFTW)(iBin)[0] + (*spectrumFFTW)(iBin)[1] * (*spectrumFFTW)(iBin)[1]));
        KTINFO(testlog, "Adding peak at bin " << iBin << "; new value: " << (*powerSpectrum)(iBin));

    }

    data.SetSpectrum(spectrum, 0);
    dataFFTW.SetSpectrum(spectrumFFTW, 0);
    dataPS.SetSpectrum(powerSpectrum, 0);

#ifdef ROOT_FOUND
    TFile* file = new TFile("spectrum_disc_test.root", "recreate");

    TH1D* histFreqSpec = KT2ROOT::CreateMagnitudeHistogram(spectrum, "hFreqSpectrum");
    histFreqSpec->SetDirectory(file);
    histFreqSpec->Write();
#endif


    // Now, discriminate
    KTVariableSpectrumDiscriminator disc;
    disc.SetMinFrequency(minFreq);
    disc.SetMaxFrequency(maxFreq);
    //disc.SetSigmaThreshold(sigmaThresh);
    disc.SetSNRPowerThreshold(snrThresh);
    disc.SetNeighborhoodRadius(1);
    disc.SetNormalize(true);

    KTINFO(testlog, "Discriminating data");
    if (! disc.Discriminate(dataPS, gvdata))
    {
        KTERROR(testlog, "Something went wrong while discriminating peaks");
        return -1;
    }
    KTDiscriminatedPoints1DData& pointData = dataPS.Of< KTDiscriminatedPoints1DData >();

    KTDiscriminatedPoints1DData::SetOfPoints setOfPoints = pointData.GetSetOfPoints(0);
    KTINFO(testlog, "Found " << setOfPoints.size() << " points above threshold");
    for (KTDiscriminatedPoints1DData::SetOfPoints::const_iterator it=setOfPoints.begin(); it != setOfPoints.end(); it++)
    {
        KTINFO(testlog, "spline[Bin]: "<<(*splineImp)(it->first));
        KTINFO(testlog, "Bin " << it->first << " = (" << it->second.fAbscissa << ", " << it->second.fOrdinate << ", "<< it->second.fMean << ", "<< it->second.fVariance << ", "<< it->second.fNeighborhoodAmplitude<<")");
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
