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

#include "KTEgg.hh"
#include "KTEggHeader.hh"
#include "KTEggReader2011.hh"
#include "KTEggReaderMonarch.hh"
#include "KTLogger.hh"

#include <iostream>
#include <string>


using namespace std;
using namespace Katydid;


KTLOGGER(testegg, "katydid.validation.egghatch");


int main(int argc, char** argv)
{

    if (argc < 2)
    {
        KTERROR(testegg, "No filename supplied");
        return 0;
    }
    string filename(argv[1]);

    string readerOption;
    if (argc >= 3)
    {
        readerOption = argv[2];
    }

    UInt_t nBundles = 5;

    KTEgg egg;
    if (readerOption == "-z" || readerOption == "--use-old-egg-reader")
    {
        KTINFO(testegg, "Using 2011 egg reader");
        KTEggReader2011* reader = new KTEggReader2011();
        egg.SetReader(reader);
    }
    else
    {
        UInt_t recordSize = 0;
        KTEggReaderMonarch* reader = new KTEggReaderMonarch();
        reader->SetTimeSeriesSizeRequest(recordSize);
        egg.SetReader(reader);
    }


    if (! egg.BreakEgg(filename))
    {
        KTERROR(testegg, "Egg file was not opened");
        return -1;
    }

    const KTEggHeader* header = egg.GetHeader();
    if (header == NULL)
    {
        KTERROR(testegg, "No header received");
        return -1;
    }
    KTINFO(testegg, "Header information:\n"
           << "\tFilename: " << header->GetFilename() << '\n'
           << "\tAcquisition Mode: " << header->GetAcquisitionMode() << '\n'
           << "\tNumber of Channels: " << header->GetNChannels() << '\n'
           << "\tSlice Size: " << header->GetSliceSize() << '\n'
           << "\tRecord Size: " << header->GetRecordSize() << '\n'
           << "\tAcquisition Time: " << header->GetAcquisitionTime() << " s\n"
           << "\tAcquisition Rate: " << header->GetAcquisitionRate() << " Hz");

    egg.CloseEgg();

    return 0;
}
