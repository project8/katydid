/*
 * TestSmoothing.cc
 *
 *  Created on: Jul 23, 2014
 *      Author: nsoblath
 *
 *  Usage: > ./TestSmoothing
 */

#include "logger.hh"
#include "KTSmooth.hh"

#ifdef ROOT_FOUND
#include "KT2ROOT.hh"
#include "TFile.h"
#include "TH2.h"
#endif

using namespace Katydid;

LOGGER(testlog, "TestSmoothing");

int main()
{
#ifndef ROOT_FOUND
    LWARN(testlog, "Testing will proceed, but since you don't have ROOT enabled, you won't see any output");
#endif

    unsigned nBins = 10;
    double value = 5.;

    //**************
    // 1-D smoothing
    //**************

    //**************
    // 2-D smoothing
    //**************
    LINFO(testlog, "Testing 2D smoothing");

    KTPhysicalArray< 2, double > array2D(nBins, 0., 1., nBins, 0., 1.);
    for (unsigned iBin = 0; iBin < nBins; ++iBin)
    {
        for (unsigned iOtherBin = 0; iOtherBin < nBins; ++iOtherBin)
        {
            array2D(iBin, iOtherBin) = 0.;
        }
        array2D(iBin, iBin) = value;
    }

#ifdef ROOT_FOUND
    TH2D* hist2Dbefore = KT2ROOT::CreateHistogram(&array2D, "array2Dbefore");
    hist2Dbefore->SetDirectory(NULL);
#endif

    if (! KTSmooth::Smooth(&array2D))
    {
        LERROR(testlog, "2D smoothing failed");
    }

#ifdef ROOT_FOUND
    TH2D* hist2Dafter = KT2ROOT::CreateHistogram(&array2D, "array2Dafter");
    hist2Dafter->SetDirectory(NULL);
#endif

    // Output the results to a ROOT file, if ROOT is available
#ifdef ROOT_FOUND
    TFile outFile("TestSmoothing.root", "recreate");

    hist2Dbefore->Write();
    hist2Dafter->Write();

    outFile.Close();
#endif

    LINFO(testlog, "Smoothing test complete");

    return 0;
}
