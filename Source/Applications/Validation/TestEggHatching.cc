/*
 * TestEggHatching.cc
 *
 *  Created on: Aug 20, 2012
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
#include "KTEvent.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesData.hh"

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

    KTINFO(testegg, "Test of hatching egg file <" << filename << ">");

    KTEgg* egg = new KTEgg();
    if (readerOption == "-z" || readerOption == "--use-old-egg-reader")
    {
        KTINFO(testegg, "Using 2011 egg reader");
        egg->SetReader(new KTEggReader2011());
    }
    else
    {
        KTINFO(testegg, "Using Monarch egg reader");
        egg->SetReader(new KTEggReaderMonarch());
    }


    KTINFO(testegg, "Opening file");
    if (egg->BreakEgg(filename))
    {
        KTINFO(testegg, "Egg opened successfully");
    }
    else
    {
        KTERROR(testegg, "Egg file was not opened");
        return -1;
    }

    const KTEggHeader* header = egg->GetHeader();
    if (header == NULL)
    {
        KTERROR(testegg, "No header received");
        return -1;
    }
    KTINFO(testegg, "Some header information:\n"
           << "\tFilename: " << header->GetFilename() << '\n'
           << "\tAcquisition Mode: " << header->GetAcquisitionMode() << '\n'
           << "\tRecord Size: " << header->GetRecordSize());

    KTINFO(testegg, "Hatching event");
    KTEvent* event = egg->HatchNextEvent();
    if (event == NULL)
    {
        KTERROR(testegg, "Event did not hatch");
        return -1;
    }

    KTTimeSeriesData* tsData = event->GetData<KTTimeSeriesData>(KTTimeSeriesData::StaticGetName());
    if (tsData == NULL)
    {
        KTWARN(testegg, "No time-series data present in event");
        return -1;
    }

    unsigned nRecords = tsData->GetNRecords();
    KTINFO(testegg, "This event contains " << nRecords << " records");
    if (nRecords >= 1)
    {
        KTINFO(testegg, "Record 0 has " << tsData->GetRecord(0).size() << " bins");
        KTINFO(testegg, "Bin 0 of record 0 is " << tsData->GetRecordAt< double >(0, 0));
    }

    KTINFO(testegg, "Test complete; cleaning up");
    egg->CloseEgg();
    delete event;
    delete egg;

    return 0;
}

