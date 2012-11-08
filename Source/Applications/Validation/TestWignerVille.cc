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
#include "KTRectangularWindow.hh"
#include "KTSlidingWindowFFTW.hh"
#include "KTSlidingWindowFSDataFFTW.hh"
#include "KTTimeSeriesChannelData.hh"
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
    UInt_t recordSize = 0;
    KTINFO(testwv, "Record size will be " << recordSize << " (if 0, it should be the same as the Monarch record size)");
    KTEggReaderMonarch* reader = new KTEggReaderMonarch();
    reader->SetTimeSeriesSizeRequest(recordSize);
    reader->SetTimeSeriesType(KTEggReaderMonarch::kFFTWTimeSeries);
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

    // Get the time-series data from the event.
    // The data is still owned by the event.
    KTTimeSeriesData* tsData = event->GetData<KTProgenitorTimeSeriesData>(KTProgenitorTimeSeriesData::StaticGetName());
    if (tsData == NULL)
    {
        KTWARN(testwv, "No time-series data present in event");
        delete event;
        egg.CloseEgg();
        return -1;
    }

    // Create the transform, and manually configure it.
    KTINFO(testwv, "Creating and configuring WV transform");
    KTWignerVille wvTransform;
    wvTransform.GetFullFFT()->SetTransformFlag("ESTIMATE");
    wvTransform.GetFullFFT()->SetSize(tsData->GetTimeSeries(0)->GetNTimeBins());
    KTEventWindowFunction* windowFunc = new KTRectangularWindow(tsData);
    windowFunc->SetSize(5000);
    wvTransform.GetWindowedFFT()->SetTransformFlag("ESTIMATE");
    wvTransform.GetWindowedFFT()->SetWindowFunction(windowFunc);
    wvTransform.GetWindowedFFT()->SetOverlap((UInt_t)0);
    wvTransform.AddPair(KTWVPair(0, 1));

    // Transform the data.
    // The data is not owned by the event because TransformData was used, not ProcessEvent.
    KTINFO(testwv, "Transforming data");
    KTSlidingWindowFSDataFFTW* wvData = wvTransform.TransformData(tsData);

    if (wvData == NULL)
    {
        KTWARN(testwv, "No data was returned by the WV transform; test failed");
    }
    else
    {
#ifdef ROOT_FOUND
        KTBasicROOTFileWriter writer;
        writer.SetFilename("WVTransformTest.root");
        writer.SetFileFlag("recreate");

        KTINFO(testwv, "Writing data to file");
        writer.Publish(wvData);
#endif
    }

    KTINFO(testwv, "Test complete; cleaning up");
    egg.CloseEgg();
    delete wvData;
    delete event;

    return 0;


}
