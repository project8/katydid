/**
 @file ExtractPowerSpectra.cc
 @brief Executable to produce 1-D power spectra
 @details Produces 1-D power spectra from time series; optionally can average multiple slices together
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#include "KTApplication.hh"
#include "KTBasicROOTFileWriter.hh"
#include "KTEggProcessor.hh"
#include "KTLogger.hh"
#include "KTSimpleFFT.hh"

#include <string>

using namespace std;
using namespace Katydid;

KTLOGGER(extpslog, "katydid.extractps");


int main(int argc, char** argv)
{
    KTApplication* app = new KTApplication(argc, argv);
    if (! app->ReadConfigFile())
    {
        KTERROR(extpslog, "Unable to read config file");
        return -1;
    }

    string appConfigName("extract-power-spectra");

    // Setup the processors and their signal/slot connections
    KTEggProcessor procEgg;
    KTSimpleFFT procFFT;
    KTBasicROOTFileWriter procPub;

    // Configure the processors
    app->Configure(&procEgg, appConfigName);
    app->Configure(&procFFT, appConfigName);
    app->Configure(&procPub, appConfigName);

    try
    {
        // this will ensure that every time procEgg hatches a slice, procFFT.ProcessTimeSeriesData will be called
        procEgg.ConnectASlot("slice", &procFFT, "ts");

        // this will ensure that when procEgg parses the header, the info is passed to PrepareFFT
        procEgg.ConnectASlot("header", &procFFT, "header");

        // this will get the output histogram when an FFT is complete
        //procFFT.ConnectASlot("fft", &powerSpectra, "get_ps");
        procFFT.ConnectASlot("fft", &procPub, "fs-polar");
    }
    catch (std::exception& e)
    {
        KTERROR(extpslog, "An error occurred while connecting signals and slots:\n"
                << '\t' << e.what());
        return -1;
    }

    // Process the egg file
    bool success = procEgg.ProcessEgg();

    if (! success) return -1;
    return 0;
}
