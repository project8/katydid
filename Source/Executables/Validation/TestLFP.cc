/*
 * TestTemplate.cc
 *
 *  Created on: Jan 26, 2021
 *      Author: N.S. Oblath
 *
 *  Tests performance of KTProcessorTemplate
 *
 *  Usage: > TestTemplate
 */

#include "KTProcessorTemplate.hh"

#include "KTDummyDataClass1.hh"
#include "KTDummyDataClass2.hh"
#include "KTDummyDataClass3.hh"

#include "KTNewDummyDataClass.hh"

#include "KTLogger.hh"
#include "KTLowPassFilter.hh"

#include "KTFrequencySpectrumDataFFTW.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTPowerSpectrumData.hh"

#include "KTLowPassFilteredData.hh"


using namespace std;
using namespace Katydid;

KTLOGGER(testlog, "TestLFP");

int main()
{
    // Create and setup processor
    KTLowPassFilter tProc;

     tProc.SetRC(3.1831e-9); // corresponds to f_c = 50 MHz


    // Parameters for filling data
    unsigned nBins = 100;
    double minFreq = 0.;
    double maxFreq = 100.e6;

    // Test processing of KTFrequencySpectrumDataFFTW

    //KTFrequencySpectrumDataFFTW tFSFFTW;

    // TODO: Fill tFSFFTW

    //tProc.Filter(tFSFFTW);

    // Check Results
    //KTLowPassFilteredFSDataFFTW& tLPFFSFFTW = tFSFFTW.Of< KTLowPassFilteredFSDataFFTW >();

    // TODO: Verify that the contents of tLPFFSFFTW are as expected


    // Test processing of KTFrequencySpectrumDataPolar

    //KTFrequencySpectrumDataPolar tFSPolar;

    // TODO: Fill tFSPolar

    //tProc.Filter(tFSPolar);

    // Check Results
    //KTLowPassFilteredFSDataPolar& tLPFFSPolar = tFSPolar.Of< KTLowPassFilteredFSDataPolar >();

    // TODO: Verify that the contents of tLPFFSPolar are as expected
    

    // Test processing of KTPowerSpectrumData

   KTPowerSpectrumData tPS;

    KTPowerSpectrum* tOnePS = new KTPowerSpectrum(nBins, minFreq, maxFreq);
    for (unsigned iBin = 0; iBin < nBins; ++iBin)
    {
        (*tOnePS)(iBin) = 1.;
    }
    tPS.SetSpectrum(tOnePS);

    tProc.Filter(tPS);

    // Check Results
    KTLowPassFilteredPSData& tLPFPS = tPS.Of< KTLowPassFilteredPSData >();
    KTPowerSpectrum* tOneLPFPS = tLPFPS.GetSpectrum();

    // TODO: Verify that the contents of tLPFPS are as expected
    //       Note that just printing out the values isn't a test that can be automated 
    for (unsigned iBin = 0; iBin < nBins; ++iBin)
    {
        KTWARN(testlog, iBin << "  " << (*tOnePS)(iBin) << " --> " << (*tOneLPFPS)(iBin))
    }


    return 0;
}
