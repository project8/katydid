/*
 * TestReverseFFTW.cc
 *
 *  Created on: Mar 8, 2019
 *      Author: ezayas
 *
 *  Will compare the time series and frequency spectrum using Parseval's theorem.
 *     (see http://en.wikipedia.org/wiki/Discrete_Fourier_transform)
 *  If ROOT is present, will draw the time series and frequency spectrum and save the histograms in TestComplexFFTW.root.
 */


#include "KT2ROOT.hh"
#include "KTReverseFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesFFTW.hh"
#include "KTTimeSeriesReal.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#include "TFile.h"
#endif

#include <cmath>
#include <iostream>

using namespace std;
using namespace Katydid;

KTLOGGER(vallog, "TestReverseFFTW");

int main()
{
    const double pi = 3.14159265359;

    const unsigned nBins = 1024;
    const double startFreq = 0.;
    const double endFreq = 10.;

    const double mult = 30.;

    KTINFO(vallog, "Testing the 1D complex-to-complex rFFT\n" <<
           "\tFrequency spectrum characteristics:\n" <<
           "\tSize: " << nBins << " bins\n" <<
           "\tRange: " << startFreq << " to " << endFreq << " Hz\n" <<
           "\tSine wave frequency: " << mult / (2.*pi) << " s\n");

    KTFrequencySpectrumFFTW* fsFFTW = new KTFrequencySpectrumFFTW(nBins, startFreq, endFreq);

    // Fill with a sinusoid
    for (unsigned iBin=0; iBin<nBins; iBin++)
    {
        fsFFTW->SetRect( iBin, cos(fsFFTW->GetBinCenter(iBin) * mult), 0.0);
    }

    // Create and prepare the FFT
    KTReverseFFTW c2crFFT;
    c2crFFT.SetTimeSize(fsFFTW->size());
    c2crFFT.SetTransformFlag("ESTIMATE");
    if (! c2crFFT.InitializeForComplexTDD())
    {
        KTERROR(vallog, "Error while initializing the FFT");
        exit(-1);
    }

    // Perform the FFT and get the results
    KTINFO(vallog, "Performing FFT");
    KTTimeSeriesFFTW* timeSeries = c2crFFT.TransformToComplex(fsFFTW);

    // Find the peak frequency
    double peakFrequency = -1.;
    double maxValue = -999999.;
    double value;
    size_t nTimeBins = timeSeries->size();

    for (unsigned iBin = 0; iBin < nTimeBins; iBin++)
    {
        value = (*timeSeries)(iBin)[0]*(*timeSeries)(iBin)[0] + (*timeSeries)(iBin)[1]*(*timeSeries)(iBin)[1];
        if (value > maxValue)
        {
            maxValue = value;
            peakFrequency = timeSeries->GetBinCenter(iBin);
        }
    }

    KTINFO(vallog, "FFT complete\n" <<
           "\tTime Series characteristics:\n" <<
           "\tSize: " << nTimeBins << " bins\n" <<
           "\tRange: " << timeSeries->GetRangeMin() << " to " << timeSeries->GetRangeMax() << " s\n" <<
           "\tBin width: " << timeSeries->GetBinWidth() << " Hz\n" <<
           "\tPeak time: " << peakFrequency << " +/- " << 0.5 * timeSeries->GetBinWidth() << " s\n");

#ifdef ROOT_FOUND
    TFile* file = new TFile("TestReverseFFTW.root", "recreate");
    TH1D* tsHist = timeSeries->CreateHistogram("hTimeSeries");
    TH1D* fsHist = KT2ROOT::CreateMagnitudeHistogram(fsFFTW, "hFreqSpect");
    tsHist->SetDirectory(file);
    fsHist->SetDirectory(file);
    tsHist->Write();
    fsHist->Write();
    //file->Close();
    //delete file;
#endif

    // Use Parseval's theorem to check the normalization of the FFT
    KTINFO(vallog, "Using Parceval's theorem to check the normalization of the transform.\n"
           "\tBoth sums should be approximately (1/2) * nBins = " << 0.5 * (double)nBins);
    // the latter is true because the average of sin^2 is 1/2, and we're effectively calculating avg(sin^2)*nbins.

    // Calculate sum(timeSeries[i]^2)
    double tsSum = 0.; // units: volts^2
    for (unsigned iBin=0; iBin<nBins; iBin++)
    {
        tsSum += (*timeSeries)(iBin)[0] * (*timeSeries)(iBin)[0] + (*timeSeries)(iBin)[1] * (*timeSeries)(iBin)[1];
    }

    KTINFO(vallog, "sum(timeSeries[i]^2) = " << tsSum << " V^2");

    // calculate (1/N) * sum(freqSpectrum[i]^2
    double fsSum = 0.; // units: volts^2
    for (unsigned iBin=0; iBin<nTimeBins; iBin++)
    {
        fsSum += fsFFTW->GetNorm(iBin);
    }

    KTINFO(vallog, "sum(freqSpectrum[i]^2) = " << fsSum << " V^2");

    double fractionalDiff = fabs(tsSum - fsSum) / (0.5 * (tsSum + fsSum));
    double threshold = 1.e-4;
    if (fractionalDiff > threshold)
    {
        KTWARN(vallog, "The two sums appear to be unequal! (|diff|/avg > " << threshold << ")\n"
                "\ttsSum - fsSum = " << tsSum - fsSum << "\n"
                "\ttsSum / fsSum = " << tsSum / fsSum << "\n"
                "\t|diff|/avg    = " << fractionalDiff);
    }
    else
    {
        KTINFO(vallog, "The two sums appear to be equal! (|diff|/avg <= " << threshold << ")\n"
                "\t|diff|/avg = " << fractionalDiff);
    }

    delete timeSeries;
    delete fsFFTW;

    return 0;

}
