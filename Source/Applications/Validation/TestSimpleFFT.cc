/*
 * TestSimpleFFT.cc
 *
 *  Created on: Dec 22, 2011
 *      Author: nsoblath
 *
 *  Usage: TestSimpleFFT -e /path/to/file.egg [-d]
 *      -e filename  -- Specify the filename
 *      -d           -- If present, will plot a histogram of the power spectrum.
 *      -z           -- If present, use the 2011 egg reader; otherwise use the monarch egg reader
 */

#include "KTEgg.hh"
#include "KTEggReader2011.hh"
#include "KTEggReaderMonarch.hh"
#include "KTEvent.hh"
#include "KTFrequencySpectrumData.hh"
#include "KTSimpleFFT.hh"
#include "KTTimeSeriesData.hh"

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
    Bool_t drawWaterfall = false;

    KTEgg egg;

    Int_t arg;
    extern char *optarg;
    while ((arg = getopt(argc, argv, "e:d:z")) != -1)
        switch (arg)
        {
            case 'e':
                inputFileName = string(optarg);
                break;
            case 'd':
                drawWaterfall = true;
                break;
            case 'z':
                egg.SetReader(new KTEggReader2011());
                break;
        }

    if (inputFileName.empty())
    {
        cout << "Error: No egg filename given" << endl;
        return -1;
    }

    if (egg.GetReader() == NULL)
    {
        egg.SetReader(new KTEggReaderMonarch());
    }

    string outputFileNamePS = outputFileNameBase + string(".ps");

    if (! egg.BreakEgg(inputFileName))
    {
        cout << "Error: Egg did not break" << endl;
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

    KTTimeSeriesData* data = event->GetData< KTTimeSeriesData >(KTTimeSeriesData::StaticGetName());
    if (data == NULL)
    {
        cout << "No data was present in the event" << endl;
        delete c1;
        return -1;
    }

    // FFT of the entire event, which will be used to normalize the gain fluctuations
    KTSimpleFFT fullFFT(data->GetRecordSize());
    fullFFT.SetTransformFlag("ESTIMATE");
    fullFFT.InitializeFFT();
    KTFrequencySpectrumData* freqData = fullFFT.TransformData(data);

    TH1D* histFullPS = freqData->GetSpectrum(0)->CreatePowerHistogram();

    if (drawWaterfall)
    {
        c1->SetLogy(1);
        histFullPS->Draw();
        c1->WaitPrimitive();
        c1->Print(outputFileNamePS.c_str());
        c1->SetLogy(0);
    }

    Bool_t histCheck = histFullPS->Integral() > 0.;
    if (histCheck)
    {
        std::cout << "The integral of the power spectrum is greater than 0!" << std::endl;
    }
    else
    {
        std::cout << "The integral of the histgoram is not greater than 0.  Something may be wrong!" << std::endl;
    }

    delete freqData;
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



