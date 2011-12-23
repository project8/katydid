/*
 * TestSimpleFFT.cc
 *
 *  Created on: Dec 22, 2011
 *      Author: nsoblath
 */

#include "KTEgg.hh"
#include "KTEvent.hh"
#include "KTPowerSpectrum.hh"
#include "KTSimpleFFT.hh"

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TROOT.h"
#include "TStyle.h"

#include <cstdio>
#include <unistd.h>
#include <iostream>


using namespace std;
using namespace Katydid;

int main(int argc, char** argv)
{
    string outputFileNameBase("test_simplefft");
    string inputFileName("");
    Bool_t drawWaterfall = kFALSE;

    Int_t arg;
    extern char *optarg;
    while ((arg = getopt(argc, argv, "e:d")) != -1)
        switch (arg)
        {
            case 'e':
                inputFileName = string(optarg);
                break;
            case 'd':
                drawWaterfall = kTRUE;
                break;
        }

    if (inputFileName.empty())
    {
        cout << "Error: No egg filename given" << endl;
        return -1;
    }

    string outputFileNamePS = outputFileNameBase + string(".ps");

    KTEgg egg;
    egg.SetFileName(inputFileName);
    if (! egg.BreakEgg())
    {
        cout << "Error: Egg did not break" << endl;
        return -1;
    }
    if (! egg.ParseEggHeader())
    {
        cout << "Error: Header did not parse" << endl;
        return -1;
    }

    TApplication* app = new TApplication("", 0, 0);
    TStyle *plain = new TStyle("Plain", "Plain Style");
    plain->SetCanvasBorderMode(0);
    plain->SetPadBorderMode(0);
    plain->SetPadColor(0);
    plain->SetCanvasColor(0);
    plain->SetTitleColor(0);
    plain->SetStatColor(0);
    plain->SetPalette(1);
    plain->SetOptStat(0);
    gROOT->SetStyle("Plain");

    TCanvas *c1 = NULL;
    Char_t tempFileName[256];
    if (drawWaterfall)
    {
        c1 = new TCanvas("c1", "c1");
        sprintf(tempFileName, "%s[", outputFileNamePS.c_str());
        c1->Print(tempFileName);
        c1->SetLogz(1);
    }

    // Hatch the event
    KTEvent* event = egg.HatchNextEvent();
    if (event == NULL)
    {
        cout << "No event hatched" << endl;
        delete c1;
        return -1;
    }

    // FFT of the entire event, which will be used to normalize the gain fluctuations
    KTSimpleFFT fullFFT(event->GetRecordSize());
    fullFFT.SetTransformFlag("ES");
    fullFFT.InitializeFFT();
    fullFFT.TakeData(event);
    fullFFT.Transform();

    KTPowerSpectrum* fullPS = fullFFT.CreatePowerSpectrum();
    TH1D* histFullPS = fullPS->CreateMagnitudeHistogram();

    if (drawWaterfall)
    {
        c1->SetLogy(1);
        histFullPS->Draw();
        c1->WaitPrimitive();
        c1->Print(outputFileNamePS.c_str());
        c1->SetLogy(0);
    }

    Bool_t histCheck = histFullPS->Integral() > 0.;

    delete fullPS;
    delete event;

    if (drawWaterfall)
    {
        sprintf(tempFileName, "%s]", outputFileNamePS.c_str());
        c1->Print(tempFileName);
        delete c1;
    }
    else
    {
        delete histFullPS;
    }

    if (! histCheck) return -1;
    return 0;
}



