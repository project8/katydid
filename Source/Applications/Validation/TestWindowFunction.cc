/*
 * TestWindowFunction.cc
 *
 *  Created on: Apr 18, 2013
 *      Author: nsoblath
 */

#include "KTLogger.hh"
#include "KTWindowFunction.hh"

#include "KTHammingWindow.hh"
#include "KTHannWindow.hh"
#include "KTRectangularWindow.hh"

#include "TFile.h"
#include "TH1.h"

using namespace Katydid;
using namespace std;

KTLOGGER(vallog, "katydid.applications.validation");

int main(int argc, char** argv)
{
    //KTRectangularWindow wf;
    //wf.SetBoxcarSize(10000);

    KTHammingWindow wf;

    UInt_t size = 16384;
    Double_t bw = 5.e-9;

    wf.SetBinWidth(bw);
    wf.SetSize(size);

#ifdef ROOT_FOUND
    TFile file("window_function_test.root", "recreate");

    TH1D* wfHist = wf.CreateHistogram();
    wfHist->SetDirectory(&file);

    TH1D* wfFRHist = wf.CreateFrequencyResponseHistogram();
    wfFRHist->SetDirectory(&file);

    wfHist->Write();
    wfFRHist->Write();

    file.Close();
#endif

    return 0;
}
