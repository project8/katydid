/*
 * TestHoughTransform.cc
 *
 *  Created on: Dec 5, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrum.hh"
#include "KTHoughTransform.hh"
#include "KTMath.hh"
#include "KTLogger.hh"
#include "KTPhysicalArray.hh"


#ifdef ROOT_FOUND
#include "TFile.h"
#include "TH2.h"
#endif


using namespace Katydid;
using namespace std;

KTLOGGER(testlog, "katydid.applications.validation");

int main()
{
    UInt_t nBinsX = 100;
    UInt_t nBinsY = 150;

    KTPhysicalArray< 1, KTFrequencySpectrum* > input(nBinsX, 0., 100.);
    for (UInt_t iPS=0; iPS<nBinsX; iPS++)
    {
        input(iPS) = new KTFrequencySpectrum(nBinsY, 0., 150.);
        // Make a horizontal line in the input
        if (iPS >= 10 && iPS < 20)
        {
            (*(input(iPS)))(83).set_polar(10., 0.);
        }
    }

#ifdef ROOT_FOUND
    TFile* file = new TFile("testHoughTransform.root", "recreate");

    TH2D* hInput = new TH2D("hInput", "Input", nBinsX, 0., 100., nBinsY, 0., 100.);
    for (UInt_t iX=0; iX<nBinsX; iX++)
    {
        KTFrequencySpectrum* xSpect = input(iX);
        for (UInt_t iY=0; iY<nBinsY; iY++)
        {
            hInput->SetBinContent(iX+1, iY+1, (*xSpect)(iY).abs());
        }
    }
    hInput->Write();
#endif

    UInt_t nThetaPoints = 400;
    UInt_t nRPoints = 100;

    KTHoughTransform hough;
    hough.SetNThetaPoints(nThetaPoints);
    hough.SetNRPoints(nRPoints);

    KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* output = hough.TransformSpectrum(&input);

#ifdef ROOT_FOUND
    Double_t maxR = (*output)(0)->GetRangeMax();
    TH2D* hOutput = new TH2D("hOutput", "Output", nThetaPoints, 0., KTMath::Pi(), nRPoints, 0., maxR);
    for (UInt_t iTheta=0; iTheta<nThetaPoints; iTheta++)
    {
        KTPhysicalArray< 1, Double_t >* thetaArray = (*output)(iTheta);
        for (UInt_t iR=0; iR<nRPoints; iR++)
        {
            hOutput->SetBinContent(iTheta+1, iR+1, (*thetaArray)(iR));
        }
    }
    hOutput->Write();
#endif



    delete output;

#ifdef ROOT_FOUND
    file->Close();
    delete file;
#endif

    return 0;
}
