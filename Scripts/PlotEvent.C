/*
 * PlotEvent.C
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 *      Summary: Plots a power spectrum from a single event
 *      Note: as of Sept 13, it was seg faulting at the InitializeFFT line, for some reason. When tested outside of this script, the code works.
 */

#include "../Source/Egg/KTEvent.hh"
#include "../Source/Egg/KTEgg.hh"
#include "../Source/Egg/KTPowerSpectrum.hh"
#include "../Source/Egg/KTSimpleFFT.hh"

#include "TH1.h"

#include <iostream>

using namespace Katydid;

void PlotEvent(const string& fileName) {

    KTEgg* egg = new KTEgg();

    egg->SetFileName(fileName);
    if (! egg->BreakEgg()) return;
    if (! egg->ParseEggHeader()) return;

    // Hatch the event
    if (! egg->HatchNextEvent()) continue;
    const KTEvent* event = egg->GetData();

    std::cout << "A" << std::endl;
    KTSimpleFFT* fft = new KTSimpleFFT((Int_t)event->GetRecordSize());
    std::cout << "B" << std::endl;
    fft->SetTransformFlag("ES");
    std::cout << "C" << std::endl;
    fft->InitializeFFT();
    std::cout << "D" << std::endl;
    fft->TakeData(event);
    std::cout << "E" << std::endl;
    fft->Transform();

    std::cout << "F" << std::endl;
    KTPowerSpectrum* ps = fft->CreatePowerSpectrum();
    std::cout << "G" << std::endl;
    delete fft;
    std::cout << "H" << std::endl;
    TH1D* hist = ps->CreateMagnitudeHistogram();
    std::cout << "I" << std::endl;
    delete ps;
    std::cout << "J" << std::endl;
    hist->Draw();
    std::cout << "K" << std::endl;

    return;
}
