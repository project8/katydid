/*
 * SimpleElectronHunt.cc
 *
 *  Created on: Oct 6, 2011
 *      Author: nsoblath
 *
 *      Search for electrons with very basic peak finding and grouping
 *
 *      Usage: SimpleElectronHunt [-e egg filename] [-p ps filename] [-n # bundles; -1 for all] [-c control case option]
 *      Command line options
 *       -e: The input data file name
 *       -p: The output file name base (there will be .root and .ps files)
 *       -n: The number of bundles to analyze; use -1 for all
 *       -t: Threshold in multiples of the mean (default is 10)
 *       -c: Use this to run one of the control setups. -1 reverses the high and low margins; -2 uses large negative margins.
 *       -d: Include this flag to draw waterfall plots
 */

#include "KTApplication.hh"
#include "KTCommandLineOption.hh"
#include "KTEggProcessor.hh"
#include "KTFFTEHunt.hh"
#include "KTLogger.hh"

#include <cstdio>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <utility>


using namespace std;
using namespace Katydid;

KTLOGGER(sehuntlog, "katydid.simpleelectronhunt");

int main(int argc, char** argv)
{
    KTApplication* app = new KTApplication(argc, argv);
    app->ReadConfigFile();

    string appConfigName("extract-power-spectra");

    KTEggProcessor procEgg;
    KTFFTEHunt procEHunt;

    /*
     * For when we can apply cut ranges
        if (cutOption == 1)
        {
            KTSetting settingHuntCut1("CutRange", pair< Double_t, Double_t > (0., 0.2));
            KTSetting settingHuntCut2("CutRange", pair< Double_t, Double_t > (99.8, 100.2));
            KTSetting settingHuntCut3("CutRange", pair< Double_t, Double_t > (199.8, 200.2));
            procEHunt.ApplySetting(&settingHuntCut1);
            procEHunt.ApplySetting(&settingHuntCut2);
            procEHunt.ApplySetting(&settingHuntCut3);
        }

     */

    // Configure the processors
    app->Configure(&procEgg, appConfigName);
    app->Configure(&procEHunt, appConfigName);

    try
    {
        // every time procEgg hatches an bundle, procEHunt.ProcessBundle will be called
        procEgg.ConnectASlot("bundle", &procEHunt, "bundle");

        // when procEgg parses the header, the info is passed to procEHunt::ProcessHeader
        procEgg.ConnectASlot("header", &procEHunt, "header");

        // when procEgg is done with the file, procEHunt is notified
        procEgg.ConnectASlot("egg-done", &procEHunt, "egg-done");
    }
    catch (std::exception& e)
    {
        KTERROR(sehuntlog, "An error occured while connecting signals and slots:\n"
                << '\t' << e.what());
        return -1;
    }



    // Open the files to add header information and remove previous contents if the files already exist
    if (procEHunt.GetWriteTextFileFlag())
    {
        ofstream outFileTxt(procEHunt.GetTextFilename().c_str(), ios::trunc);
        outFileTxt << "Egg file: " << procEgg.GetFilename() << '\n';
        outFileTxt << "------------------------------------" << '\n';
        outFileTxt.close();
    }


    // Process the file
    //-----------------
    Bool_t success = procEgg.ProcessEgg();

    if (! success) return -1;
    return 0;




}
