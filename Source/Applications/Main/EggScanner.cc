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
#include "KTEggReader2011.hh"
#include "KTEggReaderMonarch.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"

#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <iostream>
#include <string>


using namespace std;
using namespace Katydid;

using boost::shared_ptr;

KTLOGGER(eggscan, "katydid.applications.main");

static KTCommandLineOption< UInt_t > sCLNBins("Egg Scanner", "Size of the slice", "slice-size", 's');
static KTCommandLineOption< Bool_t > sScanRecords("Egg Scanner", "Scan records", "scan-records", 'r');

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

    UInt_t sliceSize = clOpts->GetCommandLineValue< unsigned >("slice-size", 16384);

    KTEggReader* reader;
    if (clOpts->IsCommandLineOptSet("use-2011-egg-reader"))
    {
        KTINFO(eggscan, "Using 2011 egg reader");
        KTEggReader2011* reader2011 = new KTEggReader2011();
        reader = reader2011;
    }
    else
    {
        KTEggReaderMonarch* readerMonarch = new KTEggReaderMonarch();
        readerMonarch->SetSliceSize(sliceSize);
        reader = readerMonarch;
    }

    Bool_t scanRecords = clOpts->IsCommandLineOptSet("scan-records");

    //**************************
    // Doing-something phase
    //**************************

    ULong64_t fileSize = boost::filesystem::file_size(filename); // in bytes

    const KTEggHeader* header = reader->BreakEgg(filename);
    if (header == NULL)
    {
        KTERROR(eggscan, "Egg file was not opened and no header was received");
        return -1;
    }

    KTPROG(eggscan, *header);

    ULong64_t recordMemorySize = header->GetSliceSize(); // each time bin is represented by 1 byte
    ULong64_t recordsInFile = fileSize / recordMemorySize; // approximate, rounding down
    ULong64_t slicesInFile = recordsInFile * ULong64_t(header->GetRecordSize() / header->GetSliceSize()); // upper limit, assuming continuous acquisition

    UInt_t fsSizeFFTW = header->GetSliceSize();
    UInt_t fsSizePolar = fsSizeFFTW / 2 + 1;
    Double_t timeBinWidth = 1. / header->GetAcquisitionRate();
    Double_t freqBinWidth = 1. / (timeBinWidth * Double_t(fsSizeFFTW));
    Double_t sliceLength = timeBinWidth * Double_t(header->GetSliceSize());
    Double_t fsMaxFreq = freqBinWidth * (Double_t(fsSizePolar) - 0.5);

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
        UInt_t iSlice = 0;
        while (kTRUE)
        {
            KTINFO(eggscan, "Hatching slice " << iSlice);

            // Hatch the slice
            shared_ptr<KTData> data = reader->HatchNextSlice();
            if (data.get() == NULL) break;

            KTPROG(eggscan, data->Of< KTSliceHeader >());

            iSlice++;
        }

    }

    reader->CloseEgg();

    return 0;
}
