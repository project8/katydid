/**
 @file ExtractPowerSpectra.cc
 @brief Executable to produce 1-D power spectra
 @details Produces 1-D power spectra from Egg events; optionally can average multiple events together
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#include "KTApplication.hh"
#include "KTBasicROOTFileWriter.hh"
//#include "KTCommandLineOption.hh"
#include "KTEggProcessor.hh"
//#include "KTEvent.hh"
#include "KTLogger.hh"
#include "KTSimpleFFT.hh"
//#include "KTFrequencySpectrumData.hh"
//#include "KTFFTTypes.hh"

//#include "TFile.h"
//#include "TH1D.h"

//#include <cmath>
//#include <cstdlib>
#include <string>
//#include <unistd.h>
//#include <vector>

using namespace std;
using namespace Katydid;

KTLOGGER(extpslog, "katydid.extractps");
/*
class PowerSpectraContainer : public KTProcessor
{
    public:
        PowerSpectraContainer();
        ~PowerSpectraContainer();
        void AddPowerSpectrum(const KTFrequencySpectrumData* fftData);
        const vector< TH1D* >& GetPowerSpectra() const;
        void ReleasePowerSpectra();
    private:
        vector< TH1D* > fPowerSpectra;
};
*/
//static KTCommandLineOption< string > sOutputFNameCLO("Extract Power Spectra", "Output filename (not including .root extension)", "output-file", 'o');
//static KTCommandLineOption< unsigned > sNAvgCLO("Extract Power Spectra", "Number of events to average together", "events-per-average", 'a');


int main(int argc, char** argv)
{
    KTApplication* app = new KTApplication(argc, argv);
    app->ReadConfigFile();

    string appConfigName("extract-power-spectra");

    // Variables to be configured either by the config file or comamnd line
    //string outputFileNameBase("power_spectra");
    //UInt_t eventsPerAverage = 1;

    // Get the application-specific configuration file options
    //KTPStoreNode* node = app->GetNode(appConfigName);
    //if (node != NULL)
    //{
        //outputFileNameBase = node->GetData< string >("output-file", outputFileNameBase);
        //eventsPerAverage = node->GetData< UInt_t >("events-per-average", eventsPerAverage);
    //}

    // Get the application-specific command line options
    //outputFileNameBase = app->GetCommandLineHandler()->GetCommandLineValue< string >("output-file", outputFileNameBase);
    //eventsPerAverage = app->GetCommandLineHandler()->GetCommandLineValue< unsigned >("events-per-average", eventsPerAverage);

    //string outputFileNameRoot = outputFileNameBase + string(".root");


    // Setup the processors and their signal/slot connections
    KTEggProcessor procEgg;
    KTSimpleFFT procFFT;
    KTBasicROOTFileWriter procPub;
    //PowerSpectraContainer powerSpectra;


    // Configure the processors
    app->Configure(&procEgg, appConfigName);
    app->Configure(&procFFT, appConfigName);
    app->Configure(&procPub, appConfigName);

    try
    {
        // this will ensure that every time procEgg hatches an event, procFFT.ProcessEvent will be called
        procEgg.ConnectASlot("event", &procFFT, "event");

        // this will ensure that when procEgg parses the header, the info is passed to PrepareFFT
        procEgg.ConnectASlot("header", &procFFT, "header");

        // this will get the output histogram when an FFT is complete
        //procFFT.ConnectASlot("fft", &powerSpectra, "get_ps");
        procFFT.ConnectASlot("fft", &procPub, "write-data");
    }
    catch (std::exception& e)
    {
        KTERROR(extpslog, "An error occurred while connecting signals and slots:\n"
                << '\t' << e.what());
        return -1;
    }



    // Process the egg file
    Bool_t success = procEgg.ProcessEgg();

    /*

    // Get the histograms out and save them to a ROOT file
    vector< TH1D* > powerSpectrumHistograms = powerSpectra.GetPowerSpectra();
    powerSpectra.ReleasePowerSpectra();

    KTINFO(extpslog, "There are " << powerSpectrumHistograms.size() << " histograms saved");

    TFile* outFile = new TFile(outputFileNameRoot.c_str(), "recreate");
    for (UInt_t iHist=0; iHist<(UInt_t)powerSpectrumHistograms.size(); iHist++)
    {
        powerSpectrumHistograms[iHist]->SetDirectory(outFile);
        powerSpectrumHistograms[iHist]->Write();
    }
    outFile->Close();
    delete outFile;
    */

    if (! success) return -1;
    return 0;
}


//**************************************
// Definitions for PowerSpectraContainer
//**************************************
/*
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

void PowerSpectraContainer::AddPowerSpectrum(const KTFrequencySpectrumData* fftData)
{
    KTEvent* event = fftData->GetEvent();

    stringstream conv;
    string histNameBase;
    conv << event->GetEventNumber();
    conv >> histNameBase;
    histNameBase = "histPS" + histNameBase;
    for (unsigned iChannel=0; iChannel<fftData->GetNChannels(); iChannel++)
    {
        conv.flush();
        conv << iChannel;
        string histName;
        conv >> histName;
        histName = histNameBase + '_' + histName;
        TH1D* powerSpectrum = CreatePowerSpectrumHistFromFreqSpect(histName, &(fftData->GetSpectrum(iChannel)));
        // set name and/or title?
        fPowerSpectra.push_back(powerSpectrum);
    }
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

*/
