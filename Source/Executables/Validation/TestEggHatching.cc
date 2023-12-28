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
#include "KTEgg1Reader.hh"
#include "logger.hh"
#include "KTSliceHeader.hh"
#include "KTTimeSeriesData.hh"

#ifdef USE_MONARCH2
#include "KTEgg2Reader.hh"
#endif

#include <iostream>


using namespace std;
using namespace Katydid;


LOGGER(testegg, "TEstEggHatching");


int main(int argc, char** argv)
{

    if (argc < 2)
    {
        LERROR(testegg, "No filename supplied");
        return 0;
    }
    string filename(argv[1]);

    string readerOption;
    if (argc >= 3)
    {
        readerOption = argv[2];
    }

    unsigned nSlices = 5;

    LINFO(testegg, "Test of hatching egg file <" << filename << ">");

    KTEggReader* reader;

    if (readerOption == "-z" || readerOption == "--use-egg1-reader")
    {
        LINFO(testegg, "Using egg1 (2011) reader");
        KTEgg1Reader* reader2011 = new KTEgg1Reader();
        reader = reader2011;
    }
    else
    {
#ifdef USE_MONARCH2
        LINFO(testegg, "Using Monarch egg reader");
        unsigned sliceSize = 500000;
        LINFO(testegg, "Slice size should be " << sliceSize);
        unsigned stride = 700000;
        LINFO(testegg, "Stride should be " << stride << " (if 0, it should be the same as the slice size");
        KTEgg2Reader* readerMonarch = new KTEgg2Reader();
        readerMonarch->SetSliceSize(sliceSize);
        readerMonarch->SetStride(stride);
        reader = readerMonarch;
#else
        LERROR(testegg, "Only the egg1 reader is enabled");
        return -1;
#endif
    }


    LINFO(testegg, "Opening file");
    Nymph::KTDataPtr headerPtr = reader->BreakAnEgg(filename);
    if (headerPtr)
    {
        LINFO(testegg, "Egg opened successfully");
    }
    else
    {
        LERROR(testegg, "Egg file was not opened");
        delete reader;
        return -1;
    }

    KTEggHeader& header = headerPtr->Of< KTEggHeader >();
    LINFO(testegg, "Some header information:\n"
           << "\tFilename: " << header.GetFilename() << '\n'
           << "\tAcquisition Mode: " << header.GetAcquisitionMode() << '\n'
           << "\tSlice Size: " << header.GetChannelHeader(0)->GetSliceSize() << '\n'
           << "\tRecord Size: " << header.GetChannelHeader(0)->GetRecordSize());

    LINFO(testegg, "Hatching slices");
    for (unsigned iSlice=0; iSlice < nSlices; iSlice++)
    {
        LINFO(testegg, "Slice " << iSlice);
        Nymph::KTDataPtr data = reader->HatchNextSlice();
        if (! data)
        {
            LERROR(testegg, "Slice did not hatch");
            return -1;
        }

        if (! data->Has< KTTimeSeriesData >())
        {
            LWARN(testegg, "No time-series data present in slice");
            return -1;
        }

        KTSliceHeader& sliceHeader = data->Of< KTSliceHeader >();
        KTTimeSeriesData& tsData = data->Of< KTTimeSeriesData >();

        LINFO(testegg, "Time in run: " << sliceHeader.GetTimeInRun() << " s");

        unsigned nRecords = tsData.GetNComponents();
        LINFO(testegg, "This slice contains " << nRecords << " records");
        if (nRecords >= 1)
        {
            LINFO(testegg, "Record 0 has " << tsData.GetTimeSeries(0)->GetNTimeBins() << " bins");
            LINFO(testegg, "Bins 0-5 of record 0 are " <<
                   tsData.GetTimeSeries(0)->GetValue(0) << "  " <<
                   tsData.GetTimeSeries(0)->GetValue(1) << "  " <<
                   tsData.GetTimeSeries(0)->GetValue(2) << "  " <<
                   tsData.GetTimeSeries(0)->GetValue(3) << "  " <<
                   tsData.GetTimeSeries(0)->GetValue(4));
        }
    }

    LINFO(testegg, "Test complete; cleaning up");
    reader->CloseEgg();
    delete reader;

    return 0;
}

