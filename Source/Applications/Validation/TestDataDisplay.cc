/*
 * TestBasicROOTFileWriter.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTDataDisplay.hh"
#include "KTDataTypeDisplayFFT.hh"
#include "KTData.hh"
#include "KTDisplayWindow.hh"
#include "KTFrequencySpectrumPolar.hh"
#include "KTFrequencySpectrumDataPolar.hh"
#include "KTSliceHeader.hh"
#include "complexpolar.hh"

#include "TH1.h"

#include <iostream>

using namespace Katydid;
using namespace std;

void ThreadCleanup(void* /*voidthread*/)
{
    return;
}

void* ThreadSetupAndExecute(void* voidthread)
{
    pthread_cleanup_push(&ThreadCleanup, voidthread);
    KTDisplayWindow* window = (KTDisplayWindow*) (voidthread);
    window->Run();
    pthread_cleanup_pop(0);
    pthread_exit(0);
}



int main()
{
    cout << "Testing KTDisplayWindow" << endl;
    {
        KTDisplayWindow* window = new KTDisplayWindow(700, 500);

        pthread_t threadID;
        pthread_create(&threadID, 0, &ThreadSetupAndExecute, window);
        usleep(500);

        TH1D* hist1 = new TH1D("hist1", "Histogram Test 1", 5, 0., 10.);
        for (UInt_t iBin = 1; iBin <= 5; ++iBin)
        {
            hist1->SetBinContent(iBin, (Double_t)iBin);
        }

        window->Draw(hist1);
        window->Stop();
        delete window;
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
