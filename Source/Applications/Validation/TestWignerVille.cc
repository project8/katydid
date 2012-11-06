/*
 * TestWignerVille.cc
 *
 *  Created on: Nov 6, 2012
 *      Author: nsoblath
 *
 *  Usage:
 *      > TestWignerVille filename.egg
 */

#include "KTComplexFFTW.hh"
#include "KTEgg.hh"
#include "KTEggHeader.hh"
#include "KTEggReaderMonarch.hh"
#include "KTEvent.hh"
#include "KTLogger.hh"
#include "KTTimeSeriesChannelData.hh"
#include "KTSlidingWindowFFTW.hh"
#include "KTSlidingWindowFSDataFFTW.hh"
#include "KTWignerVille.hh"

#ifdef ROOT_FOUND
#include "KTBasicROOTFileWriter.hh"
#endif


using namespace std;
using namespace Katydid;


KTLOGGER(testwv, "katydid.validation");


int main(int argc, char** argv)
{

    if (argc < 2)
    {
        KTERROR(testwv, "No filename supplied");
        return 0;
    }
    string filename(argv[1]);

    KTINFO(testwv, "Test of hatching egg file <" << filename << ">");

    KTEgg egg;
    UInt_t recordSize = 500000;
    KTINFO(testwv, "Record size will be " << recordSize << " (if 0, it should be the same as the Monarch record size)");
    KTEggReaderMonarch* reader = new KTEggReaderMonarch();
    reader->SetTimeSeriesSizeRequest(recordSize);
    egg.SetReader(reader);

    KTINFO(testwv, "Opening file");
    if (egg.BreakEgg(filename))
    {
        KTINFO(testwv, "Egg opened successfully");
    }
    else
    {
        KTERROR(testwv, "Egg file was not opened");
        return -1;
    }

    const KTEggHeader* header = egg.GetHeader();
    if (header == NULL)
    {
        KTERROR(testwv, "No header received");
        egg.CloseEgg();
        return -1;
    }

    KTINFO(testwv, "Hatching event");
    KTEvent* event = egg.HatchNextEvent();
    if (event == NULL)
    {
        KTERROR(testwv, "Event did not hatch");
        egg.CloseEgg();
        return -1;
    }

    KTTimeSeriesData* tsData = event->GetData<KTProgenitorTimeSeriesData>(KTProgenitorTimeSeriesData::StaticGetName());
    if (tsData == NULL)
    {
        KTWARN(testwv, "No time-series data present in event");
        delete event;
        egg.CloseEgg();
        return -1;
    }

    KTINFO(testwv, "Creating and configuring WV tranform");
    KTWignerVille wvTransform;
    wvTransform.GetFullFFT()->SetTransformFlag("ESTIMATE");
    wvTransform.GetWindowedFFT()->SetTransformFlag("ESTIMATE");
    wvTransform.GetWindowedFFT()->SetWindowSize(5000);
    wvTransform.GetWindowedFFT()->SetOverlap((UInt_t)0);

    KTINFO(testwv, "Transforming data");
    KTSlidingWindowFSDataFFTW* wvData = wvTransform.TransformData(tsData);

#ifdef ROOT_FOUND
    KTBasicROOTFileWriter writer;
    writer.SetFilename("WVTransformTest.root");
    writer.SetFileFlag("recreate");

    KTINFO(testwv, "Writing data to file");
    writer.Publish(wvData);
#endif

    KTINFO(testwv, "Test complete; cleaning up");
    egg.CloseEgg();
    delete wvData;
    delete tsData;
    delete event;

    return 0;


}
