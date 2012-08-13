/*
 * SimpleElectronHunt.cc
 *
 *  Created on: Oct 6, 2011
 *      Author: nsoblath
 *
 *      Search for electrons with very basic peak finding and grouping
 *
 *      Usage: SimpleElectronHunt [-e egg filename] [-p ps filename] [-n # events; -1 for all] [-c control case option]
 *      Command line options
 *       -e: The input data file name
 *       -p: The output file name base (there will be .root and .ps files)
 *       -n: The number of events to analyze; use -1 for all
 *       -t: Threshold in multiples of the mean (default is 10)
 *       -c: Use this to run one of the control setups. -1 reverses the high and low margins; -2 uses large negative margins.
 *       -d: Include this flag to draw waterfall plots
 */

#include "KTEggProcessor.hh"
#include "KTFFTEHuntProcessor.hh"
#include "KTSetting.hh"

#include "TApplication.h"
#include "TROOT.h"
#include "TStyle.h"

#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <cstdio>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <utility>


using namespace std;
using namespace Katydid;


int main(int argc, char** argv)
{
    string outputFileNameBase("candidates");
    string inputFileName("");
    Int_t numEvents = 1;

    Double_t thresholdMult = 10.;

    Bool_t drawWaterfall = kFALSE;

    Int_t groupBinsMarginLow = 1;
    Int_t groupBinsMarginHigh = 3;
    Int_t groupBinsMarginSameTime = 1;

    Int_t cutOption = 0;

    UInt_t firstBinToUse = 4;

    Int_t arg;
    extern char *optarg;
    while ((arg = getopt(argc, argv, "e:p:n:t:c:d")) != -1)
        switch (arg)
        {
            case 'e':
                inputFileName = string(optarg);
                break;
            case 'p':
                outputFileNameBase = string(optarg);
                break;
            case 'n':
                numEvents = atoi(optarg);
                break;
            case 't':
                thresholdMult = atof(optarg);
                break;
            case 'c':
            {
                cutOption = atoi(optarg);
                break;
            }
            case 'd':
                drawWaterfall = kTRUE;
                break;
        }

    if (inputFileName.empty())
    {
        cout << "Error: No egg filename given" << endl;
        return -1;
    }

    string outputFileNameRoot = outputFileNameBase + string(".root");
    string outputFileNamePS = outputFileNameBase + string(".ps");
    string outputFileNameText = outputFileNameBase + string(".txt");

    if (numEvents == -1) numEvents = 999999999;

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


    //ofstream txtOutFile(outputFileNameText.c_str());
    //txtOutFile << "Egg file: " << inputFileName << endl;
    //txtOutFile << "------------------------------------" << endl;

    // Setup the processors and their signal/slot connections
    //-------------------------------------------------------
    KTEggProcessor procEgg;
    KTSetting settingEggNEvents("NEvents", (UInt_t)numEvents);
    procEgg.ApplySetting(&settingEggNEvents);

    KTFFTEHuntProcessor procEHunt;
    KTSetting settingHuntThresholdMult("ThresholdMult", (Double_t)thresholdMult);
    KTSetting settingHuntGroupBinsMarginHigh("GroupBinsMarginHigh", (Int_t)groupBinsMarginHigh);
    KTSetting settingHuntGroupBinsMarginLow("GroupBinsMarginLow", (Int_t)groupBinsMarginLow);
    KTSetting settingHuntGroupBinsMarginSameTime("GroupBinsMarginSameTime", (Int_t)groupBinsMarginSameTime);
    KTSetting settingHuntFirstBinToUse("FirstBinToUse", (UInt_t)firstBinToUse);
    KTSetting settingHuntWriteText("WriteTextFileFlag", kTRUE);
    KTSetting settingHuntTextFilename("TextFilename", outputFileNameText);
    KTSetting settingHuntWriteROOT("WriteROOTFileFlag", drawWaterfall);
    KTSetting settingHuntROOTFilename("ROOTFilename", outputFileNameRoot);
    procEHunt.ApplySetting(&settingHuntThresholdMult);
    procEHunt.ApplySetting(&settingHuntGroupBinsMarginHigh);
    procEHunt.ApplySetting(&settingHuntGroupBinsMarginLow);
    procEHunt.ApplySetting(&settingHuntGroupBinsMarginSameTime);
    procEHunt.ApplySetting(&settingHuntFirstBinToUse);
    procEHunt.ApplySetting(&settingHuntWriteText);
    procEHunt.ApplySetting(&settingHuntTextFilename);
    procEHunt.ApplySetting(&settingHuntWriteROOT);
    procEHunt.ApplySetting(&settingHuntROOTFilename);

    if (cutOption == 1)
    {
        KTSetting settingHuntCut1("CutRange", pair< Double_t, Double_t > (0., 0.2));
        KTSetting settingHuntCut2("CutRange", pair< Double_t, Double_t > (99.8, 100.2));
        KTSetting settingHuntCut3("CutRange", pair< Double_t, Double_t > (199.8, 200.2));
        procEHunt.ApplySetting(&settingHuntCut1);
        procEHunt.ApplySetting(&settingHuntCut2);
        procEHunt.ApplySetting(&settingHuntCut3);
    }

    // this will ensure that every time procEgg hatches an event, procEHunt.ProcessEvent will be called
    //procFFT.ConnectToEventSignalFrom(procEgg);
    //procEHunt.SetEventSlotConnection(procEgg.ConnectToEventSignal( boost::bind(&KTFFTEHuntProcessor::ProcessEvent, boost::ref(procEHunt), _1, _2) ));
    procEHunt.SetEventSlotConnection(procEgg.ConnectToSignal< void (UInt_t, const KTEvent*) >("event", boost::bind(&KTFFTEHuntProcessor::ProcessEvent, boost::ref(procEHunt), _1, _2)));

    // this will ensure that when procEgg parses the header, the info is passed to procEHunt::ProcessHeader
    //procFFT.ConnectToEventSignalFrom(procEgg);
    //procEHunt.SetHeaderSlotConnection(procEgg.ConnectToHeaderSignal( boost::bind(&KTFFTEHuntProcessor::ProcessHeader, boost::ref(procEHunt), _1) ));
    procEHunt.SetHeaderSlotConnection(procEgg.ConnectToSignal< void (KTEgg::HeaderInfo) >("header", boost::bind(&KTFFTEHuntProcessor::ProcessHeader, boost::ref(procEHunt), _1)));
    //procEHunt.SetHeaderSlotConnection(procEgg.ConnectToSignal< void (KTEgg::HeaderInfo) >("testsig", boost::bind(&KTFFTEHuntProcessor::ProcessHeader, boost::ref(procEHunt), _1) ));

    // this will ensure that when procEgg is done with the file, procEHunt is notified
    //procEHunt.SetEggDoneSlotConnection(procEgg.ConnectToEggDoneSignal( boost::bind(&KTFFTEHuntProcessor::FinishHunt, boost::ref(procEHunt)) ));
    procEHunt.SetEggDoneSlotConnection(procEgg.ConnectToSignal< void () >("egg_done", boost::bind(&KTFFTEHuntProcessor::FinishHunt, boost::ref(procEHunt))));

    // Open the files to add header information and remove previous contents if the files already exist
    ofstream outFileTxt(outputFileNameText.c_str(), ios::trunc);
    outFileTxt << "Egg file: " << inputFileName << endl;
    outFileTxt << "------------------------------------" << endl;
    outFileTxt.close();
    if (drawWaterfall)
    {
        TFile f(outputFileNameRoot.c_str(), "recreate");
        f.Close();
    }

    // Process the file
    //-----------------
    Bool_t success = procEgg.ProcessEgg(inputFileName);

    if (! success) return -1;
    return 0;

}
