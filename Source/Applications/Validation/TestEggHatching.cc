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
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"

#include <iostream>


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

    UInt_t nSlices = 5;

    KTINFO(testegg, "Test of hatching egg file <" << filename << ">");

    KTEgg egg;
    if (readerOption == "-z" || readerOption == "--use-old-egg-reader")
    {
        KTINFO(testegg, "Using 2011 egg reader");
        KTEggReader2011* reader = new KTEggReader2011();
        egg.SetReader(reader);
    }
    else
    {
        KTINFO(testegg, "Using Monarch egg reader");
        UInt_t sliceSize = 500000;
        KTINFO(testegg, "Slice size should be " << sliceSize);
        UInt_t stride = 700000;
        KTINFO(testegg, "Stride should be " << stride << " (if 0, it should be the same as the slice size");
        KTEggReaderMonarch* reader = new KTEggReaderMonarch();
        reader->SetSliceSize(sliceSize);
        reader->SetStride(stride);
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
           << "\tSlice Size: " << header->GetSliceSize() << '\n'
           << "\tRecord Size: " << header->GetRecordSize());

    KTINFO(testegg, "Hatching slices");
    for (UInt_t iSlice=0; iSlice < nSlices; iSlice++)
    {
        KTINFO(testegg, "Slice " << iSlice);
        boost::shared_ptr<KTData> data = egg.HatchNextSlice();
        if (! data)
        {
            KTERROR(testegg, "Slice did not hatch");
            return -1;
        }

        if (! data->Has< KTTimeSeriesData >())
        {
            KTWARN(testegg, "No time-series data present in slice");
            return -1;
        }

        KTSliceHeader& sliceHeader = data->Of< KTSliceHeader >();
        KTTimeSeriesData& tsData = data->Of< KTTimeSeriesData >();

        KTINFO(testegg, "Time in run: " << sliceHeader.GetTimeInRun() << " s");

        UInt_t nRecords = tsData.GetNComponents();
        KTINFO(testegg, "This slice contains " << nRecords << " records");
        if (nRecords >= 1)
        {
            KTINFO(testegg, "Record 0 has " << tsData.GetTimeSeries(0)->GetNTimeBins() << " bins");
            KTINFO(testegg, "Bin 0 of record 0 is " << tsData.GetTimeSeries(0)->GetValue(0));
        }
    }

    KTINFO(testegg, "Test complete; cleaning up");
    egg.CloseEgg();

    return 0;
}

