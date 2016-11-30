/*
 * ProfileFFTWandMonarch.cc
 *
 *  Created on: Dec 21, 2012
 *      Author: nsoblath
 */

#include "KTData.hh"
#include "KTEggHeader.hh"
#include "KTLogger.hh"
#include "KTThroughputProfiler.hh"

#include "M2Monarch.hh"
#include "M2Exception.hh"

#include <fftw3.h>

#include <cstdlib>

using namespace std;
using namespace Katydid;
using namespace monarch2;

KTLOGGER(proflog, "ProfileFFTWandMonarch");

int main(const int argc, const char** argv)
{
    if (argc < 2)
    {
        KTWARN(proflog, "Usage:\n" <<
                "\tProfileFFTWandMonarch <input egg file> <# of slices to read>");
        return -1;
    }

    unsigned nSlices = atoi(argv[2]);

    const Monarch2* tReadTest = Monarch2::OpenForReading(argv[1]);
    try
    {
        tReadTest->ReadHeader();
    }
    catch (M2Exception& e)
    {
        KTERROR(proflog, "could not read header: " << e.what());
        return -1;
    }

    const M2Header* tReadHeader = tReadTest->GetHeader();
    KTEggHeader tEggHeader;
    tEggHeader.SetFilename(tReadHeader->GetFilename());
    tEggHeader.SetAcquisitionMode(tReadHeader->GetAcquisitionMode());
    tEggHeader.SetNChannels(2);
    tEggHeader.GetChannelHeader(0)->SetRecordSize(tReadHeader->GetRecordSize());
    tEggHeader.GetChannelHeader(0)->SetSliceSize(tReadHeader->GetRecordSize());
    tEggHeader.SetRunDuration(tReadHeader->GetRunDuration());
    tEggHeader.SetAcquisitionRate(tReadHeader->GetAcquisitionRate() * 1.e6);

    KTDEBUG(proflog, "Parsed header:\n"
         << "\tFilename: " << tEggHeader.GetFilename() << '\n'
         << "\tAcquisition Mode: " << tEggHeader.GetAcquisitionMode() << '\n'
         << "\tNumber of Channels: " << tEggHeader.GetNChannels() << '\n'
         << "\tRecord Size: " << tEggHeader.GetChannelHeader(0)->GetSliceSize() << '\n'
         << "\tRecord Size: " << tEggHeader.GetChannelHeader(0)->GetRecordSize() << '\n'
         << "\tRun Duration: " << tEggHeader.GetRunDuration() << " ms" << '\n'
         << "\tAcquisition Rate: " << tEggHeader.GetAcquisitionRate() << " Hz ");

    unsigned tSize = tEggHeader.GetChannelHeader(0)->GetRecordSize();

    KTINFO(proflog, "File opened and header extracted successfully (" << tSize << ")");

    // Dummy data pointer
    Nymph::KTDataPtr dataPtr(new Nymph::KTData());

    // Create FFT
    KTINFO(proflog, "Setting up the FFT");
    fftw_complex* tInputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tSize);
    fftw_complex* tOutputArray = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * tSize);
    fftw_plan tPlan = fftw_plan_dft_1d(tSize, tInputArray, tOutputArray, FFTW_FORWARD, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);

    KTINFO(proflog, "FFT setup complete");

    // Create the profiler
    KTThroughputProfiler profiler;

    // Start the timer!
    KTINFO(proflog, "Starting profiling");
    profiler.Start();

    const M2RecordBytes* tRecord1 = tReadTest->GetRecordSeparateOne();
    const M2RecordBytes* tRecord2 = tReadTest->GetRecordSeparateTwo();
    const M2RecordDataInterface< uint64_t > tData1( tRecord1->fData, tEggHeader.GetChannelHeader(0)->GetDataTypeSize() );
    const M2RecordDataInterface< uint64_t > tData2( tRecord2->fData, tEggHeader.GetChannelHeader(0)->GetDataTypeSize() );

    for (unsigned iSlice=0; iSlice < nSlices; iSlice++)
    {
        KTINFO(proflog, "Slice " << iSlice);
        if (tReadTest->ReadRecord() == false)
        {
            KTERROR(proflog, "Problem reading records at slice " << iSlice);
            break;
        }

        // first record
        // copy the data
        for (unsigned index=0; index < tReadHeader->GetRecordSize(); index++)
        {
            tInputArray[index][0] = double(tData1.at(index));
        }
        // perform the fft
        fftw_execute_dft(tPlan, tInputArray, tOutputArray);

        // second record
        // copy the data
        for (unsigned index=0; index < tReadHeader->GetRecordSize(); index++)
        {
            tInputArray[index][0] = double(tData2.at(index));
        }
        // perform the fft
        fftw_execute_dft(tPlan, tInputArray, tOutputArray);

        profiler.Data(dataPtr);
    }

    // Stop the timer and print info
    profiler.Finish();

    // clean up
    fftw_destroy_plan(tPlan);
    fftw_free(tInputArray);
    fftw_free(tOutputArray);

    tReadTest->Close();
    delete tReadTest;

    return 0;
}




