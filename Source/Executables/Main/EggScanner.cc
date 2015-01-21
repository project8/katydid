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

#include <boost/filesystem.hpp>

#include <iostream>
#include <string>


using namespace std;
using namespace Katydid;



KTLOGGER(eggscan, "EggScanner");

static KTCommandLineOption< unsigned > sCLNBins("Egg Scanner", "Size of the slice", "slice-size", 's');
static KTCommandLineOption< bool > sScanRecords("Egg Scanner", "Scan records", "scan-records", 'r');

int main(int argc, char** argv)
{
    //**************************
    // Configuration phase
    //**************************

    KTApplication* app = new KTApplication(argc, argv);
    KTCommandLineHandler* clOpts = app->GetCommandLineHandler();
    clOpts->DelayedCommandLineProcessing();

    if (! clOpts->IsCommandLineOptSet("egg-file"))
    {
        KTERROR(eggscan, "No filename supplied; please specify with -e [filename]");
        return 0;
    }
    string filename(clOpts->GetCommandLineValue< string >("egg-file"));

    unsigned sliceSize = clOpts->GetCommandLineValue< unsigned >("slice-size", 16384);

    KTEggReader* reader;
    if (clOpts->IsCommandLineOptSet("use-egg1-reader"))
    {
        KTINFO(eggscan, "Using egg1 (2011) reader");
        KTEgg1Reader* reader2011 = new KTEgg1Reader();
        reader = reader2011;
    }
    else
    {
#ifdef USE_MONARCH2
        KTEgg2Reader* readerMonarch = new KTEgg2Reader();
        readerMonarch->SetSliceSize(sliceSize);
        reader = readerMonarch;
#else
        KTERROR(eggscan, "Can only use Egg1 reader unless Monarch2 is enabled");
#endif
    }

    bool scanRecords = clOpts->IsCommandLineOptSet("scan-records");

    //**************************
    // Doing-something phase
    //**************************

    uint64_t fileSize = boost::filesystem::file_size(filename); // in bytes

    KTDataPtr headerPtr = reader->BreakEgg(filename);
    if (! headerPtr)
    {
        KTERROR(eggscan, "Egg file was not opened and no header was received");
        return -1;
    }

    KTEggHeader& header = headerPtr->Of< KTEggHeader >();
    KTPROG(eggscan, header);

    uint64_t recordMemorySize = header.GetSliceSize(); // each time bin is represented by 1 byte
    uint64_t recordsInFile = fileSize / recordMemorySize; // approximate, rounding down
    uint64_t slicesInFile = recordsInFile * uint64_t(header.GetRecordSize() / header.GetSliceSize()); // upper limit, assuming continuous acquisition

    unsigned fsSizeFFTW = header.GetSliceSize();
    unsigned fsSizePolar = fsSizeFFTW / 2 + 1;
    double timeBinWidth = 1. / header.GetAcquisitionRate();
    double freqBinWidth = 1. / (timeBinWidth * double(fsSizeFFTW));
    double sliceLength = timeBinWidth * double(header.GetSliceSize());
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
            KTDataPtr data = reader->HatchNextSlice();
            if (data.get() == NULL) break;

            KTPROG(eggscan, data->Of< KTSliceHeader >());

            iSlice++;
        }

    }

    reader->CloseEgg();

    return 0;
}
