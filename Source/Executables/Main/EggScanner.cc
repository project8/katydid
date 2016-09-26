/*
 * EggScanner.cc
 *
 *  Created on: Jan 31, 2013
 *      Author: nsoblath
 *
 *  Usage:
 *     for reading 2012+ data: bin/TestEggHatching filename.egg
 *     for reading 2011  data: bin/TestEggHatching filename.egg -z
 */

#include "KTApplication.hh"
#include "KTCommandLineOption.hh"
#include "KTData.hh"
#include "KTEggHeader.hh"
#include "KTEgg1Reader.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"

#ifdef USE_MONARCH2
#include "KTEgg2Reader.hh"
#endif

#ifdef USE_MONARCH3
#include "KTEgg3Reader.hh"
#endif

#include <boost/filesystem.hpp>

#include <iostream>
#include <string>

using namespace std;
using namespace Katydid;

KTLOGGER(eggscan, "EggScanner");

static Nymph::KTCommandLineOption< bool > sUseEgg1("Egg Scanner", "Use the Egg1 reader", "egg1", '1');
static Nymph::KTCommandLineOption< bool > sUseEgg2("Egg Scanner", "Use the Egg2 reader", "egg2", '2');
static Nymph::KTCommandLineOption< bool > sUseEgg3("Egg Scanner", "Use the Egg3 reader", "egg3", '3');

static Nymph::KTCommandLineOption< unsigned > sCLNBins("Egg Scanner", "Size of the slice", "slice-size", 's');
static Nymph::KTCommandLineOption< bool > sScanRecords("Egg Scanner", "Scan records", "scan-records", 'r');

int main(int argc, char** argv)
{
    //**************************
    // Configuration phase
    //**************************

    Nymph::KTApplication* app = new Nymph::KTApplication(argc, argv);
    Nymph::KTCommandLineHandler* clOpts = app->GetCommandLineHandler();
    clOpts->DelayedCommandLineProcessing();

    if (! clOpts->IsCommandLineOptSet("egg-file"))
    {
        KTERROR(eggscan, "No filename supplied; please specify with -e [filename]");
        return 0;
    }
    string filename(clOpts->GetCommandLineValue< string >("egg-file"));

    unsigned sliceSize = clOpts->GetCommandLineValue< unsigned >("slice-size", 16384);

    KTEggReader* reader;
    if (clOpts->IsCommandLineOptSet("egg1"))
    {
        KTINFO(eggscan, "Using egg1 reader");
        KTEgg1Reader* reader2011 = new KTEgg1Reader();
        reader = reader2011;
    }
    else if (clOpts->IsCommandLineOptSet("egg2"))
    {
#ifdef USE_MONARCH2
        KTINFO(eggscan, "Using egg2 reader");
        KTEgg2Reader* readerMonarch = new KTEgg2Reader();
        readerMonarch->SetSliceSize(sliceSize);
        reader = readerMonarch;
#else
        KTERROR(eggscan, "Can only use Egg2 reader if Monarch2 is enabled");
        return 0;
#endif
    }
    else if (clOpts->IsCommandLineOptSet("egg3"))
    {
#ifdef USE_MONARCH3
        KTINFO(eggscan, "Using egg3 reader");
        KTEgg3Reader* readerMonarch = new KTEgg3Reader();
        readerMonarch->SetSliceSize(sliceSize);
        reader = readerMonarch;
#else
        KTERROR(eggscan, "Can only use Egg3 reader if Monarch3 is enabled");
        return 0;
#endif
    }
    else
    {
        KTERROR(eggscan, "Please specify the egg reader type");
        return 0;
    }

    bool scanRecords = clOpts->IsCommandLineOptSet("scan-records");

    //**************************
    // Doing-something phase
    //**************************

    uint64_t fileSize = boost::filesystem::file_size(filename); // in bytes

    Nymph::KTDataPtr headerPtr = reader->BreakEgg(filename);
    if (! headerPtr)
    {
        KTERROR(eggscan, "Egg file was not opened and no header was received");
        return -1;
    }

    KTEggHeader& header = headerPtr->Of< KTEggHeader >();
    KTPROG(eggscan, header);

    uint64_t recordMemorySize = header.GetChannelHeader(0)->GetRecordSize(); // each time bin is represented by 1 byte
    uint64_t recordsInFile = fileSize / recordMemorySize; // approximate, rounding down
    uint64_t slicesInFile = recordsInFile * uint64_t(header.GetChannelHeader(0)->GetRecordSize() / header.GetChannelHeader(0)->GetSliceSize()); // upper limit, assuming continuous acquisition

    unsigned fsSizeFFTW = header.GetChannelHeader(0)->GetSliceSize();
    unsigned fsSizePolar = fsSizeFFTW / 2 + 1;
    double timeBinWidth = 1. / header.GetAcquisitionRate();
    double freqBinWidth = 1. / (timeBinWidth * double(fsSizeFFTW));
    double sliceLength = timeBinWidth * double(header.GetChannelHeader(0)->GetSliceSize());
    double fsMaxFreq = freqBinWidth * (double(fsSizePolar) - 0.5);

    KTPROG(eggscan, "Additional information:\n"
           << "\tFile size: " << fileSize/1000000 << " MB\n"
           << "\tRecords in the file: " << recordsInFile << '\n'
           << "\tSlices in the file: " << slicesInFile << "  (upper limit assuming continuous acquisition)\n"
           << "\tTime bin width: " << timeBinWidth << " s\n"
           << "\tSlice length: " << sliceLength << " s\n"
           << "\tFrequency bin width: " << freqBinWidth << " Hz\n"
           << "\tFS size (FFTW): " << fsSizeFFTW << '\n'
           << "\tFS size (polar): " << fsSizePolar << '\n'
           << "\tMax frequency: " << fsMaxFreq << " Hz");

    if (scanRecords)
    {
        unsigned iSlice = 0;
        while (true)
        {
            KTINFO(eggscan, "Hatching slice " << iSlice);

            // Hatch the slice
            Nymph::KTDataPtr data = reader->HatchNextSlice();
            if (data.get() == NULL) break;

            KTPROG(eggscan, data->Of< KTSliceHeader >());

            iSlice++;
        }

    }

    reader->CloseEgg();

    return 0;
}
