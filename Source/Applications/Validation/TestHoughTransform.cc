/*
 * TestHoughTransform.cc
 *
 *  Created on: Dec 5, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumPolar.hh"
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

KTLOGGER(testlog, "TestHoughTransform");

int main()
{
    unsigned nBinsX = 100;
    unsigned nBinsY = 150;

    KTPhysicalArray< 1, KTFrequencySpectrumPolar* > input(nBinsX, 0., 100.);
    for (unsigned iPS=0; iPS<nBinsX; iPS++)
    {
        input(iPS) = new KTFrequencySpectrumPolar(nBinsY, 0., 150.);
        // Make a horizontal line in the input
        if (iPS >= 10 && iPS < 20)
        {
            (*(input(iPS)))(83).set_polar(10., 0.);
        }
    }

#ifdef ROOT_FOUND
    TFile* file = new TFile("testHoughTransform.root", "recreate");

    TH2D* hInput = new TH2D("hInput", "Input", nBinsX, 0., 100., nBinsY, 0., 100.);
    for (unsigned iX=0; iX<nBinsX; iX++)
    {
        KTFrequencySpectrumPolar* xSpect = input(iX);
        for (unsigned iY=0; iY<nBinsY; iY++)
        {
            hInput->SetBinContent(iX+1, iY+1, (*xSpect)(iY).abs());
        }
    }
    hInput->Write();
#endif

    unsigned nThetaPoints = 400;
    unsigned nRPoints = 100;

    KTHoughTransform hough;
    hough.SetNThetaPoints(nThetaPoints);
    hough.SetNRPoints(nRPoints);

    KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* output = hough.TransformSpectrum(&input);

#ifdef ROOT_FOUND
    double maxR = (*output)(0)->GetRangeMax();
    TH2D* hOutput = new TH2D("hOutput", "Output", nThetaPoints, 0., KTMath::Pi(), nRPoints, 0., maxR);
    for (unsigned iTheta=0; iTheta<nThetaPoints; iTheta++)
    {
        KTPhysicalArray< 1, double >* thetaArray = (*output)(iTheta);
        for (unsigned iR=0; iR<nRPoints; iR++)
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
