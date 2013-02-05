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
#include "KTBundle.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesChannelData.hh"

#include <boost/shared_ptr.hpp>

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

    UInt_t nEvents = 5;

    KTINFO(testegg, "Test of hatching egg file <" << filename << ">");

    KTEgg egg;
    if (readerOption == "-z" || readerOption == "--use-old-egg-reader")
    {
        KTINFO(testegg, "Using 2011 egg reader");
        KTEggReader2011* reader = new KTEggReader2011();
        reader->SetOutputDataName("time-series");
        egg.SetReader(reader);
    }
    else
    {
        KTINFO(testegg, "Using Monarch egg reader");
        UInt_t recordSize = 0;
        KTINFO(testegg, "Record size should be " << recordSize << " (if 0, it should be the same as the Monarch record size)");
        KTEggReaderMonarch* reader = new KTEggReaderMonarch();
        reader->SetTimeSeriesSizeRequest(recordSize);
        reader->SetOutputDataName("time-series");
        egg.SetReader(reader);
    }


    KTINFO(testegg, "Opening file");
    if (egg.BreakEgg(filename))
    {
        KTINFO(testegg, "Egg opened successfully");
    }
    else
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
    KTINFO(testegg, "Some header information:\n"
           << "\tFilename: " << header->GetFilename() << '\n'
           << "\tAcquisition Mode: " << header->GetAcquisitionMode() << '\n'
           << "\tRecord Size: " << header->GetRecordSize() << '\n'
           << "\tMonarch Record Size: " << header->GetMonarchRecordSize());

    KTINFO(testegg, "Hatching bundles");
    for (UInt_t iEvent=0; iEvent < nEvents; iEvent++)
    {
        KTINFO(testegg, "Event " << iEvent);
        boost::shared_ptr<KTBundle> bundle = egg.HatchNextEvent();
        if (bundle.get() == NULL)
        {
            KTERROR(testegg, "Event did not hatch");
            return -1;
        }

        KTTimeSeriesData* tsData = bundle->GetData<KTTimeSeriesData>("time-series");
        if (tsData == NULL)
        {
            KTWARN(testegg, "No time-series data present in bundle");
            return -1;
        }

        UInt_t nRecords = tsData->GetNTimeSeries();
        KTINFO(testegg, "This bundle contains " << nRecords << " records");
        if (nRecords >= 1)
        {
            KTINFO(testegg, "Record 0 has " << tsData->GetTimeSeries(0)->GetNTimeBins() << " bins");
            KTINFO(testegg, "Bin 0 of record 0 is " << tsData->GetTimeSeries(0)->GetValue(0));
        }
    }

    KTINFO(testegg, "Test complete; cleaning up");
    egg.CloseEgg();

    return 0;
}

