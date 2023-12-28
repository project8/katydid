/*
 * TestWindowFunction.cc
 *
 *  Created on: Apr 18, 2013
 *      Author: nsoblath
 */

#include "logger.hh"
#include "KTWindowFunction.hh"

#include "KTBlackmanHarrisWindow.hh"
#include "KTHammingWindow.hh"
#include "KTHannWindow.hh"
#include "KTRectangularWindow.hh"
#include "KTSincWindow.hh"

#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH1.h"
#endif

using namespace Katydid;
using namespace std;

LOGGER(vallog, "TestWindowFunction");

int main()
{
    //KTRectangularWindow wf;
    //wf.SetBoxcarSize(10000);

    //KTHammingWindow wf;

    //KTBlackmanHarrisWindow wf;

    KTSincWindow wf;

    unsigned size = 16384;
    double bw = 5.e-9;

    wf.SetBinWidth(bw);
    wf.SetSize(size);

#ifdef ROOT_FOUND
    TFile file("window_function_test.root", "recreate");

    TH1D* wfHist = wf.CreateHistogram();
    wfHist->SetDirectory(&file);
    wfHist->Write();

#ifdef FFTW_FOUND
    TH1D* wfFRHist = wf.CreateFrequencyResponseHistogram();
    wfFRHist->SetDirectory(&file);
    wfFRHist->Write();
#endif

    file.Close();
#endif

    return 0;
}
