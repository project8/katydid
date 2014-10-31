/*
 * TestDataDisplay.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTApplication.hh"
#include "KTDataDisplay.hh"
#include "KTDataTypeDisplayFFT.hh"
#include "KTData.hh"
#include "KTDisplayWindow.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTRootGuiLoop.hh"
#include "KTSliceHeader.hh"
#include "complexpolar.hh"

#include "TCanvas.h"
#include "TH1.h"

#include <iostream>



using namespace Katydid;
using namespace std;


int main()
{
    KTApplication fApp(true);

    cout << "Testing KTDisplayWindow" << endl;
    {
        KTRootGuiLoop* loop = new KTRootGuiLoop();
        fApp.AddEventLoop(loop);

        KTDisplayWindow* window = new KTDisplayWindow(700, 500);
        window->Connect("Cancel()", "Katydid::KTRootGuiLoop", loop, "Stop()");
        window->Connect("Continue()", "Katydid::KTRootGuiLoop", loop, "Pause()");

        TCanvas* canv = window->GetCanvas();

        TH1D* hist1 = new TH1D("hist1", "Histogram Test 1", 5, 0., 10.);
        double offset = 0.;
        //while(window->IsActive())
        for (unsigned hcount = 0; hcount < 2 && loop->IsActive(); ++hcount)
        {
            for (unsigned iBin = 1; iBin <= 5; ++iBin)
            {
                hist1->SetBinContent(iBin, (double)iBin + offset);
            }
            offset += 2;
            hist1->Draw();
            canv->Update();

            loop->Go();
        }

        delete window;

        fApp.RemoveEventLoop(loop);
        delete loop;
    }


    cout << "Test of KTDisplayWindow complete" << endl;
    char resp = ' ';
    while (resp != 'y' && resp != 'n')
    {
        cout << "Continue with KTDataDisplay test? (y/n)  ";
        cin >> resp;
    }
    if (resp == 'n') return 0;


    cout << "\nTesting KTDataDisplay" << endl;
    {
        // Set up the data
        KTDataPtr data(new KTData);

        KTSliceHeader& header = data->Of< KTSliceHeader >();
        header.SetSliceNumber(1);

        KTFrequencySpectrumDataPolar& fsData = data->Of< KTFrequencySpectrumDataPolar >().SetNComponents(2);

        KTFrequencySpectrumPolar* spectrum1 = new KTFrequencySpectrumPolar(10, -0.5, 9.5);
        (*spectrum1)(3).set_polar(5., 1.);
        fsData.SetSpectrum(spectrum1, 0);

        KTFrequencySpectrumPolar* spectrum2 = new KTFrequencySpectrumPolar(10, -0.5, 9.5);
        (*spectrum2)(8).set_polar(3., 2.);
        fsData.SetSpectrum(spectrum2, 1);

        // Set up the writer
        KTDataDisplay display;
        display.SetWidth(700);
        display.SetHeight(500);

        // Writer the data
        display.GetTypeWriter< KTDataTypeDisplayFFT >()->DrawFrequencySpectrumDataPolar(data);

        // Set up next data
        (*spectrum1)(3).set_polar(10., .5);
        (*spectrum2)(8).set_polar(12., 2.1);
        header.SetSliceNumber(2);

        // Publish the data
        display.GetTypeWriter< KTDataTypeDisplayFFT >()->DrawFrequencySpectrumDataPolar(data);
    }


    cout << "Test complete" << endl;

    return 0;

}
