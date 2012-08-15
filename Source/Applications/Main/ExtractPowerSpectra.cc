/**
 @file ExtractPowerSpectra.cc
 @brief Executable to produce 1-D power spectra
 @details Produces 1-D power spectra from Egg events; optionally can average multiple events together
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#include "KTEggProcessor.hh"
#include "KTSimpleFFTProcessor.hh"
#include "KTSetting.hh"

#include "TFile.h"
#include "TH1D.h"

//#include <boost/bind.hpp>
//#include <boost/signals2.hpp>

#include <cmath>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <vector>

#include <iostream>

using namespace std;
using namespace Katydid;

class PowerSpectraContainer
{
    public:
        PowerSpectraContainer();
        ~PowerSpectraContainer();
        void AddPowerSpectrum(UInt_t iEvent, const KTSimpleFFT* fft);
        const vector< TH1D* >& GetPowerSpectra() const;
        void ReleasePowerSpectra();
    private:
        vector< TH1D* > fPowerSpectra;
};

int main(int argc, char** argv)
{
    string outputFileNameBase("power_spectra");
    string inputFileName("");
    UInt_t numEvents = 1;
    UInt_t eventsPerAverage = 1;

    Int_t arg;
    extern char *optarg;
    while ((arg = getopt(argc, argv, "e:p:n:a")) != -1)
        switch (arg)
        {
            case 'e':
                cout << optarg << endl;
                inputFileName = string(optarg);
                break;
            case 'p':
                cout << optarg << endl;
                outputFileNameBase = string(optarg);
                break;
            case 'n':
                cout << optarg << endl;
                numEvents = (UInt_t)abs(atoi(optarg));
                break;
            case 'a':
                cout << optarg << endl;
                eventsPerAverage = (UInt_t)abs(atof(optarg));
                break;
        }

    if (inputFileName.empty())
    {
        cout << "Error: No egg filename given" << endl;
        return -1;
    }

    string outputFileNameRoot = outputFileNameBase + string(".root");

    // Setup the processors and their signal/slot connections
    KTEggProcessor procEgg;
    KTSetting settingEggNEvents("NEvents", numEvents);
    procEgg.ApplySetting(&settingEggNEvents);

    KTSimpleFFTProcessor procFFT;
    KTSetting settingFFTTransFlag("TransformFlag", string("ES"));
    procFFT.ApplySetting(&settingFFTTransFlag);

    PowerSpectraContainer powerSpectra;

    try
    {
        // this will ensure that every time procEgg hatches an event, procFFT.ProcessEvent will be called
        //procFFT.ConnectToEventSignalFrom(procEgg);
        //procFFT.SetEventSlotConnection(procEgg.ConnectToSignal< void (UInt_t, const KTEvent*) >("event", boost::bind(&KTSimpleFFTProcessor::ProcessEvent, boost::ref(procFFT), _1, _2)));
        procEgg.ConnectToSignal2< KTSimpleFFTProcessor, void (UInt_t, const KTEvent*) >("event", &procFFT, "event");

        // this will ensure that when procEgg parses the header, the info is passed to PrepareFFT
        //procFFT.ConnectToEventSignalFrom(procEgg);
        //procFFT.SetHeaderSlotConnection(procEgg.ConnectToSignal< void (KTEgg::HeaderInfo) >("header", boost::bind(&KTSimpleFFTProcessor::ProcessHeader, boost::ref(procFFT), _1)));
        procEgg.ConnectToSignal2< KTSimpleFFTProcessor, void (KTEgg::HeaderInfo) >("header", &procFFT, "header");
    }
    catch (std::exception& e)
    {
        std::cout << "An error occured while connecting signals and slots:" << std::endl;
        std::cout << e.what() << endl;
    }
    // get the output histogram when an FFT is complete
    boost::signals2::connection fftConnection = procFFT.ConnectToFFTSignal( boost::bind(&PowerSpectraContainer::AddPowerSpectrum, boost::ref(powerSpectra), _1, _2) );

    Bool_t success = procEgg.ProcessEgg(inputFileName);

    fftConnection.disconnect();

    vector< TH1D* > powerSpectrumHistograms = powerSpectra.GetPowerSpectra();
    powerSpectra.ReleasePowerSpectra();

    cout << "There are " << powerSpectrumHistograms.size() << " histograms saved" << endl;

    TFile* outFile = new TFile(outputFileNameRoot.c_str(), "recreate");
    for (UInt_t iHist=0; iHist<(UInt_t)powerSpectrumHistograms.size(); iHist++)
    {
        powerSpectrumHistograms[iHist]->SetDirectory(outFile);
        powerSpectrumHistograms[iHist]->Write();
    }
    outFile->Close();
    delete outFile;

    if (! success) return -1;
    return 0;
}


//**************************************
// Definitions for PowerSpectraContainer
//**************************************

PowerSpectraContainer::PowerSpectraContainer() :
        fPowerSpectra()
{
}

PowerSpectraContainer::~PowerSpectraContainer()
{
    while (! fPowerSpectra.empty())
    {
        delete fPowerSpectra.back();
        fPowerSpectra.pop_back();
    }
}

void PowerSpectraContainer::AddPowerSpectrum(UInt_t iEvent, const KTSimpleFFT* fft)
{
    stringstream conv;
    string histName;
    conv << iEvent;
    conv >> histName;
    histName = "histPS" + histName;
    TH1D* powerSpectrum = fft->CreatePowerSpectrumHistogram(histName);
    // set name and/or title?
    fPowerSpectra.push_back(powerSpectrum);
    return;
}

const vector< TH1D* >& PowerSpectraContainer::GetPowerSpectra() const
{
    return fPowerSpectra;
}

void PowerSpectraContainer::ReleasePowerSpectra()
{
    fPowerSpectra.clear();
    return;
}
