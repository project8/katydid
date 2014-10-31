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


#include "KTEggHeader.hh"
#include "KTEggReader2011.hh"
#include "KTEggReaderMonarch.hh"
#include "KTLogger.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"

#include <iostream>


using namespace std;
using namespace Katydid;


KTLOGGER(testegg, "TEstEggHatching");


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

    unsigned nSlices = 5;

    KTINFO(testegg, "Test of hatching egg file <" << filename << ">");

    KTEggReader* reader;

    if (readerOption == "-z" || readerOption == "--use-old-egg-reader")
    {
        KTINFO(testegg, "Using 2011 egg reader");
        KTEggReader2011* reader2011 = new KTEggReader2011();
        reader = reader2011;
    }
    else
    {
        KTINFO(testegg, "Using Monarch egg reader");
        unsigned sliceSize = 500000;
        KTINFO(testegg, "Slice size should be " << sliceSize);
        unsigned stride = 700000;
        KTINFO(testegg, "Stride should be " << stride << " (if 0, it should be the same as the slice size");
        KTEggReaderMonarch* readerMonarch = new KTEggReaderMonarch();
        readerMonarch->SetSliceSize(sliceSize);
        readerMonarch->SetStride(stride);
        reader = readerMonarch;
    }


    KTINFO(testegg, "Opening file");
    KTDataPtr headerPtr = reader->BreakEgg(filename);
    if (headerPtr)
    {
        KTINFO(testegg, "Egg opened successfully");
    }
    else
    {
        KTERROR(testegg, "Egg file was not opened");
        delete reader;
        return -1;
    }

    KTEggHeader& header = headerPtr->Of< KTEggHeader >();
    KTINFO(testegg, "Some header information:\n"
           << "\tFilename: " << header.GetFilename() << '\n'
           << "\tAcquisition Mode: " << header.GetAcquisitionMode() << '\n'
           << "\tSlice Size: " << header.GetSliceSize() << '\n'
           << "\tRecord Size: " << header.GetRecordSize());

    KTINFO(testegg, "Hatching slices");
    for (unsigned iSlice=0; iSlice < nSlices; iSlice++)
    {
        KTINFO(testegg, "Slice " << iSlice);
        KTDataPtr data = reader->HatchNextSlice();
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

        unsigned nRecords = tsData.GetNComponents();
        KTINFO(testegg, "This slice contains " << nRecords << " records");
        if (nRecords >= 1)
        {
            KTINFO(testegg, "Record 0 has " << tsData.GetTimeSeries(0)->GetNTimeBins() << " bins");
            KTINFO(testegg, "Bins 0-5 of record 0 are " <<
                   tsData.GetTimeSeries(0)->GetValue(0) << "  " <<
                   tsData.GetTimeSeries(0)->GetValue(1) << "  " <<
                   tsData.GetTimeSeries(0)->GetValue(2) << "  " <<
                   tsData.GetTimeSeries(0)->GetValue(3) << "  " <<
                   tsData.GetTimeSeries(0)->GetValue(4));
        }
    }

    KTINFO(testegg, "Test complete; cleaning up");
    reader->CloseEgg();
    delete reader;

    return 0;
}

