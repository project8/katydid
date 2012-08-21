/**
 @file ExtractPowerSpectra.cc
 @brief Executable to produce 1-D power spectra
 @details Produces 1-D power spectra from Egg events; optionally can average multiple events together
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#include "KTApplication.hh"
#include "KTCommandLineOption.hh"
#include "KTEggProcessor.hh"
#include "KTSimpleFFT.hh"
#include "KTSetting.hh"

#include "TFile.h"
#include "TH1D.h"

#include <cmath>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <vector>

#include <iostream>

using namespace std;
using namespace Katydid;

class PowerSpectraContainer : public KTProcessor
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

static KTCommandLineOption< string > sOutputFNameCLO("Extract Power Spectra", "Output filename (not including .root extension)", "output-file", 'o');
static KTCommandLineOption< unsigned > sNAvgCLO("Extract Power Spectra", "Number of events to average together", "events-per-average", 'a');


int main(int argc, char** argv)
{
    KTApplication* app = new KTApplication(argc, argv);
    app->ReadConfigFile();

    string appConfigName("extract-power-spectra");

    // Variables to be configured either by the config file or comamnd line
    string outputFileNameBase("power_spectra");
    UInt_t eventsPerAverage = 1;

    // Get the application-specific configuration file options
    KTPStoreNode* node = app->GetNode(appConfigName);
    if (node != NULL)
    {
        outputFileNameBase = node->GetData< string >("output-file", outputFileNameBase);
        eventsPerAverage = node->GetData< UInt_t >("events-per-average", eventsPerAverage);
    }

    // Get the application-specific command line options
    outputFileNameBase = app->GetCommandLineHandler()->GetCommandLineValue< string >("output-file", outputFileNameBase);
    eventsPerAverage = app->GetCommandLineHandler()->GetCommandLineValue< unsigned >("events-per-average", eventsPerAverage);

    string outputFileNameRoot = outputFileNameBase + string(".root");


    // Setup the processors and their signal/slot connections
    KTEggProcessor procEgg;
    KTSimpleFFT procFFT;
    PowerSpectraContainer powerSpectra;


    // Configure the processors
    app->Configure(&procEgg, appConfigName);
    app->Configure(&procFFT, appConfigName);

    try
    {
        // this will ensure that every time procEgg hatches an event, procFFT.ProcessEvent will be called
        procEgg.ConnectASlot("event", &procFFT, "event");

        // this will ensure that when procEgg parses the header, the info is passed to PrepareFFT
        procEgg.ConnectASlot("header", &procFFT, "header");

        // this will get the output histogram when an FFT is complete
        procFFT.ConnectASlot("fft", &powerSpectra, "get_ps");
    }
    catch (std::exception& e)
    {
        std::cout << "An error occured while connecting signals and slots:" << std::endl;
        std::cout << e.what() << endl;
        return -1;
    }



    // Process the egg file
    Bool_t success = procEgg.ProcessEgg();



    // Get the histograms out and save them to a ROOT file
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
        KTProcessor(),
        fPowerSpectra()
{
    this->RegisterSlot("get_ps", this, &PowerSpectraContainer::AddPowerSpectrum);
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
