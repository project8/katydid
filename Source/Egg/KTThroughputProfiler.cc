/*
 * KTThroughputProfiler.cc
 *
 *  Created on: Nov 30, 2012
 *      Author: nsoblath
 */

#include "KTThroughputProfiler.hh"

#include "KTEggHeader.hh"
#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include "MonarchTypes.hpp"

#include <sstream>

#ifdef __MACH__
#include <mach/mach_time.h>

#define MACNANO (+1.0E-9)
#define MACGIGA UINT64_C(1000000000)
#endif



using std::string;
using std::stringstream;
using std::vector;

namespace Katydid
{
    KTLOGGER(proflog, "katydid.egg");

    static KTDerivedNORegistrar< KTProcessor, KTThroughputProfiler > sTProfRegistrar("throughput-profiler");

    KTThroughputProfiler::KTThroughputProfiler(const std::string& name) :
            KTProcessor(name),
            fOutputFileFlag(false),
            fOutputFilename("throughput.json"),
            fEggHeader(),
            fTimeStart(),
            fTimeEnd(),
            fNDataProcessed(0),
            fMacTimebase(0.0),
            fMacTimestart(0)
    {
        RegisterSlot("start", this, &KTThroughputProfiler::ProcessHeader);
        RegisterSlot("data", this, &KTThroughputProfiler::ProcessData);
        RegisterSlot("stop", this, &KTThroughputProfiler::Finish);
    };

    KTThroughputProfiler::~KTThroughputProfiler()
    {
    };

    Bool_t KTThroughputProfiler::Configure(const KTPStoreNode* node)
    {
        SetOutputFileFlag(node->GetData< Bool_t >("output-file-flag", fOutputFileFlag));
        SetOutputFilename(node->GetData< string >("output-filename-base", fOutputFilename));

        return true;
    }

    void KTThroughputProfiler::Start()
    {
        fTimeStart = CurrentTime();
        KTDEBUG(proflog, "Start time: " << fTimeStart.tv_sec << " sec and " << fTimeStart.tv_nsec << " nsec");
        return;
    }

    void KTThroughputProfiler::Stop()
    {
        fTimeEnd = CurrentTime();
        KTDEBUG(proflog, "End time: " << fTimeEnd.tv_sec << " sec and " << fTimeEnd.tv_nsec << " nsec");
        return;
    }

    timespec KTThroughputProfiler::Elapsed()
    {
        return Diff(fTimeStart, fTimeEnd);
    }

    void KTThroughputProfiler::ProcessHeader(const KTEggHeader* header)
    {
        fEggHeader = *header;
        KTINFO(proflog, "Profiling started");
        fNDataProcessed = 0;
        Start();
        return;
    }

    void KTThroughputProfiler::ProcessData(KTDataPtr data)
    {
        (void)data;
        fNDataProcessed++;
        return;
    }

    void KTThroughputProfiler::Finish()
    {
        Stop();
        KTINFO(proflog, "Profiling stopped");
        timespec diffTime = Elapsed();
        KTPROG(proflog, fNDataProcessed << " slices processed");
        double totalSeconds = double(diffTime.tv_sec) + double(diffTime.tv_nsec) * 1.e-9;
        KTPROG(proflog, "Throughput time: " << diffTime.tv_sec << " sec and " << diffTime.tv_nsec << " nsec (" << totalSeconds << " sec)");

        // Data production rate in bytes per second
        double dataProductionRate = double(fEggHeader.GetNChannels()) * fEggHeader.GetAcquisitionRate() * double(sizeof(DataType));

        // Data throughput rate in bytes per second
        double dataThroughputRate = 0.;
        if (totalSeconds != 0)
            dataThroughputRate = double(fEggHeader.GetSliceSize() * fEggHeader.GetNChannels() * fNDataProcessed * sizeof(DataType)) / totalSeconds;

        KTINFO(proflog, "Data production rate: " << dataProductionRate << " bytes per second");
        KTINFO(proflog, "Data throughput rate: " << dataThroughputRate << " bytes per second");
        KTPROG(proflog, "Analysis time factor: " << dataProductionRate / dataThroughputRate);

        return;
    }

    timespec KTThroughputProfiler::CurrentTime()
    {
        timespec ts;
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        if (!fMacTimestart) {
            mach_timebase_info_data_t tb = { .numer = 0, .denom = 1 };
            mach_timebase_info(&tb);
            fMacTimebase = tb.numer;
            fMacTimebase /= tb.denom;
            fMacTimestart = mach_absolute_time();
        }
        double diff = (mach_absolute_time() - fMacTimestart) * fMacTimebase;
        ts.tv_sec = diff * MACNANO;
        ts.tv_nsec = diff - (ts.tv_sec * MACGIGA);
#else
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
#endif
        return ts;
    }

    timespec KTThroughputProfiler::Diff(timespec start, timespec end) const
    {
        timespec temp;
        if ((end.tv_nsec - start.tv_nsec < 0)){
            temp.tv_sec = end.tv_sec - start.tv_sec - 1;
            temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
        }
        else
        {
            temp.tv_sec = end.tv_sec - start.tv_sec;
            temp.tv_nsec = end.tv_nsec - start.tv_nsec;
        }
        return temp;
    }

} /* namespace Katydid */
