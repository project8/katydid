/*
 * TestComplexFFTW.cc
 *
 *  Created on: Dec 22, 2011
 *      Author: nsoblath
 *
 *  Will compare the time series and frequency spectrum using Parseval's theorem.
 *     (see http://en.wikipedia.org/wiki/Discrete_Fourier_transform)
 *  If ROOT is present, will draw the time series and frequency spectrum and save the histograms in TestComplexFFTW.root.
 *
 *  Usage: > TestComplexFFTW
 */


#include "KT2ROOT.hh"
#include "KTForwardFFTW.hh"
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

KTLOGGER(vallog, "TestForwardFFTW");

int main()
{
    const double pi = 3.14159265359;

    const unsigned nBins = 1024;
    const double startTime = 0.;
    const double endTime = 10.;

    const double mult = 30.;

    KTINFO(vallog, "Testing the 1D real-to-complex and real-as-complex-to-complex FFTs\n" <<
           "\tTime series characteristics:\n" <<
           "\tSize: " << nBins << " bins\n" <<
           "\tRange: " << startTime << " to " << endTime << " s\n" <<
           "\tSine wave frequency: " << mult / (2.*pi) << " Hz\n");

    KTTimeSeriesReal* timeSeries = new KTTimeSeriesReal(nBins, startTime, endTime);
    KTTimeSeriesReal* timeSeries2 = new KTTimeSeriesReal(nBins, startTime, endTime);

    // Fill the time series with a sinusoid.
    // The units are volts.
    for (unsigned iBin=0; iBin<nBins; iBin++)
    {
        (*timeSeries)(iBin) = sin(timeSeries->GetBinCenter(iBin) * mult);
        (*timeSeries2)(iBin) = sin(timeSeries2->GetBinCenter(iBin) * mult);
        //KTDEBUG(vallog, iBin << "  " << (*timeSeries)(iBin));
    }

    // Create and prepare the FFT
    KTForwardFFTW r2cFFT;
    r2cFFT.SetTimeSize(timeSeries->size());
    r2cFFT.SetTransformFlag("ESTIMATE");
    if (! r2cFFT.InitializeForRealTDD())
    {
        KTERROR(vallog, "Error while initializing the R2C FFT");
        exit(-1);
    }

    KTForwardFFTW rasc2cFFT;
    rasc2cFFT.SetTimeSize(timeSeries2->size());
    rasc2cFFT.SetTransformFlag("ESTIMATE");
    if (! rasc2cFFT.InitializeForRealAsComplexTDD())
    {
        KTERROR(vallog, "Error while initializing the RasC2C FFT");
        exit(-1);
    }

    // Perform the FFT and get the results
    KTINFO(vallog, "Performing FFT");
    KTFrequencySpectrumFFTW* frequencySpectrum = r2cFFT.Transform(timeSeries);
    KTFrequencySpectrumFFTW* frequencySpectrum2 = rasc2cFFT.TransformAsComplex(timeSeries2);
    //size_t nFreqBins2 = frequencySpectrum2->size();

    // Evaluate frequencySpectrum (r2c transform)

    // Find the peak frequency
    double peakFrequency = -1.;
    double maxValue = -999999.;
    double value;
    size_t nFreqBins = frequencySpectrum->size();

    for (unsigned iBin = 0; iBin < nFreqBins; iBin++)
    {
        value = (*frequencySpectrum)(iBin)[0]*(*frequencySpectrum)(iBin)[0] + (*frequencySpectrum)(iBin)[1]*(*frequencySpectrum)(iBin)[1];
        if (value > maxValue)
        {
            maxValue = value;
            peakFrequency = frequencySpectrum->GetBinCenter(iBin);
        }
    }

    KTINFO(vallog, "R2C FFT complete\n" <<
           "\tFrequency spectrum characteristics:\n" <<
           "\tSize: " << nFreqBins << " bins\n" <<
           "\tRange: " << frequencySpectrum->GetRangeMin() << " to " << frequencySpectrum->GetRangeMax() << " Hz\n" <<
           "\tBin width: " << frequencySpectrum->GetBinWidth() << " Hz\n" <<
           "\tPeak frequency: " << peakFrequency << " +/- " << 0.5 * frequencySpectrum->GetBinWidth() << " Hz\n");

#ifdef ROOT_FOUND
    TFile* file = new TFile("TestForwardFFTW.root", "recreate");
    TH1D* tsHist = timeSeries->CreateHistogram("hTimeSeries");
    TH1D* fsHistR2C = KT2ROOT::CreateMagnitudeHistogram(frequencySpectrum, "hFreqSpectR2C");
    tsHist->SetDirectory(file);
    fsHistR2C->SetDirectory(file);
    tsHist->Write();
    fsHistR2C->Write();
    //file->Close();
    //delete file;
#endif

    // Use Parseval's theorem to check the normalization of the FFT
    KTINFO(vallog, "Using Parceval's theorem to check the normalization of the R2C transform.\n"
           "\tBoth sums should be approximately (1/2) * nBins = " << 0.5 * (double)nBins);
    // the latter is true because the average of sin^2 is 1/2, and we're effectively calculating avg(sin^2)*nbins.

    // Calculate sum(timeSeries[i]^2)
    double tsSum = 0.; // units: volts^2
    for (unsigned iBin=0; iBin<nBins; iBin++)
    {
        tsSum += (*timeSeries)(iBin) * (*timeSeries)(iBin);
    }

    KTINFO(vallog, "sum(timeSeries[i]^2) = " << tsSum << " V^2");

    // calculate (1/N) * sum(freqSpectrum[i]^2
    double fsSum = 0.; // units: volts^2
    for (unsigned iBin=0; iBin<nFreqBins; iBin++)
    {
        fsSum += (*frequencySpectrum)(iBin)[0] * (*frequencySpectrum)(iBin)[0] + (*frequencySpectrum)(iBin)[1] * (*frequencySpectrum)(iBin)[1];
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

    // Evaluate frequencySpectrum2 (rasc2c transform)

    // Find the peak frequency
    peakFrequency = -1.;
    maxValue = -999999.;
    nFreqBins = frequencySpectrum2->size();

    for (unsigned iBin = 0; iBin < nFreqBins; iBin++)
    {
        value = (*frequencySpectrum2)(iBin)[0]*(*frequencySpectrum2)(iBin)[0] + (*frequencySpectrum2)(iBin)[1]*(*frequencySpectrum2)(iBin)[1];
        if (value > maxValue)
        {
            maxValue = value;
            peakFrequency = frequencySpectrum2->GetBinCenter(iBin);
        }
    }

    KTINFO(vallog, "FFT complete\n" <<
           "\tFrequency spectrum characteristics:\n" <<
           "\tSize: " << nFreqBins << " bins\n" <<
           "\tRange: " << frequencySpectrum2->GetRangeMin() << " to " << frequencySpectrum2->GetRangeMax() << " Hz\n" <<
           "\tBin width: " << frequencySpectrum2->GetBinWidth() << " Hz\n" <<
           "\tPeak frequency: " << peakFrequency << " +/- " << 0.5 * frequencySpectrum2->GetBinWidth() << " Hz\n");

#ifdef ROOT_FOUND
    //TFile* file = new TFile("TestForwardFFTW.root", "recreate");
    //TH1D* tsHist = timeSeries->CreateHistogram("hTimeSeries");
    TH1D* fsHistRasC2C = KT2ROOT::CreateMagnitudeHistogram(frequencySpectrum2, "hFreqSpectRasC2C");
    //tsHist->SetDirectory(file);
    fsHistRasC2C->SetDirectory(file);
    //tsHist->Write();
    fsHistRasC2C->Write();
    file->Close();
    delete file;
#endif

    // Use Parseval's theorem to check the normalization of the FFT
    KTINFO(vallog, "Using Parceval's theorem to check the normalization of the RasC2C transform.\n"
           "\tBoth sums should be approximately (1/2) * nBins = " << 0.5 * (double)nBins);
    // the latter is true because the average of sin^2 is 1/2, and we're effectively calculating avg(sin^2)*nbins.

    // Calculate sum(timeSeries[i]^2)
    tsSum = 0.; // units: volts^2
    for (unsigned iBin=0; iBin<nBins; iBin++)
    {
        tsSum += (*timeSeries)(iBin) * (*timeSeries)(iBin);
    }

    KTINFO(vallog, "sum(timeSeries[i]^2) = " << tsSum << " V^2");

    // calculate (1/N) * sum(freqSpectrum[i]^2
    fsSum = 0.; // units: volts^2
    for (unsigned iBin=0; iBin<nFreqBins; iBin++)
    {
        fsSum += (*frequencySpectrum2)(iBin)[0] * (*frequencySpectrum2)(iBin)[0] + (*frequencySpectrum2)(iBin)[1] * (*frequencySpectrum2)(iBin)[1];
    }

    KTINFO(vallog, "sum(freqSpectrum2[i]^2) = " << fsSum << " V^2");

    fractionalDiff = fabs(tsSum - fsSum) / (0.5 * (tsSum + fsSum));
    threshold = 1.e-4;
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
    delete frequencySpectrum;
    delete frequencySpectrum2;

    return 0;

}



