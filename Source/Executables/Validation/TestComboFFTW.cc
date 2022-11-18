/*
 * TestComboFFTW.cc
 *
 *  Created on: Mar 8, 2019
 *      Author: ezayas
 *
 *  Will compare the time series before and after a forward and reverse FFT
 */


#include "KT2ROOT.hh"
#include "KTForwardFFTW.hh"
#include "KTReverseFFTW.hh"
#include "KTFrequencySpectrumFFTW.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesFFTW.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#include "TFile.h"
#endif

#include <cmath>
#include <iostream>

using namespace std;
using namespace Katydid;

KTLOGGER(vallog, "TestComboFFTW");

int main()
{
    const double pi = 3.14159265359;

    const unsigned nBins = 1024;
    const double startTime = 0.;
    const double endTime = 10.;

    const double mult = 30.;

    KTINFO(vallog, "Testing the forward and reverse FFTs\n" <<
           "\tTime series characteristics:\n" <<
           "\tSize: " << nBins << " bins\n" <<
           "\tRange: " << startTime << " to " << endTime << " s\n" <<
           "\tSine wave frequency: " << mult / (2.*pi) << " Hz\n");

    KTTimeSeriesFFTW* timeSeries = new KTTimeSeriesFFTW(nBins, startTime, endTime);

    // Fill the time series with a sinusoid.
    // The units are volts.
    for (unsigned iBin=0; iBin<nBins; iBin++)
    {
        timeSeries->SetRect(iBin, cos(timeSeries->GetBinCenter(iBin) * mult), 0.0);
    }

    // Create and prepare the FFTs

    KTForwardFFTW forwardFFT;
    KTReverseFFTW reverseFFT;

    forwardFFT.SetComplexAsIQ( true );
    forwardFFT.SetTimeSize(timeSeries->size());
    forwardFFT.SetTransformFlag("ESTIMATE");
    reverseFFT.SetTimeSize(timeSeries->size());
    reverseFFT.SetTransformFlag("ESTIMATE");

    if (! forwardFFT.InitializeForComplexTDD())
    {
        KTERROR(vallog, "Error while initializing the forward FFT");
        exit(-1);
    }

    if (! reverseFFT.InitializeForComplexTDD())
    {
        KTERROR(vallog, "Error while initializing the reverse FFT");
        exit(-1);
    }

    // Perform the FFTs

    KTINFO(vallog, "Performing FFT");

    KTFrequencySpectrumFFTW* frequencySpectrum = forwardFFT.Transform(timeSeries);
    KTTimeSeriesFFTW* newTimeSeries = reverseFFT.TransformToComplex( frequencySpectrum );
    
#ifdef ROOT_FOUND
    TFile* file = new TFile("TestComboFFTW.root", "recreate");
    TH1D* tsHist = timeSeries->CreateHistogram("hTimeSeries");
    TH1D* tsHistNew = newTimeSeries->CreateHistogram("hTimeSeriesNew");
    tsHist->SetDirectory(file);
    tsHistNew->SetDirectory(file);
    tsHist->Write();
    tsHistNew->Write();
#endif

    delete timeSeries;
    delete newTimeSeries;
    delete frequencySpectrum;

    return 0;

}
