/*
 * KTThroughputProfiler.cc
 *
 *  Created on: Nov 30, 2012
 *      Author: nsoblath
 */

#include "KTThroughputProfiler.hh"

#include "KTParam.hh"

#include <sstream>


using std::string;
using std::stringstream;
using std::vector;

namespace Nymph
{
    KTLOGGER(proflog, "KTThroughputProfiler");

    KT_REGISTER_PROCESSOR(KTThroughputProfiler, "throughput-profiler");

    KTThroughputProfiler::KTThroughputProfiler(const std::string& name) :
            KTProcessor(name),
            fOutputFileFlag(false),
            fOutputFilename("throughput.json"),
            fTimeStart(),
            fTimeEnd(),
            fNDataProcessed(0)
    {
        RegisterSlot("start", this, &KTThroughputProfiler::StartProfiling);
        RegisterSlot("data", this, &KTThroughputProfiler::Data);
        RegisterSlot("stop", this, &KTThroughputProfiler::Finish);
    };

    KTThroughputProfiler::~KTThroughputProfiler()
    {
    };

    bool KTThroughputProfiler::Configure(const KTParamNode* node)
    {
        SetOutputFileFlag(node->GetValue< bool >("output-file-flag", fOutputFileFlag));
        SetOutputFilename(node->GetValue("output-filename-base", fOutputFilename));

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

    void KTThroughputProfiler::StartProfiling(KTDataPtr header)
    {
        KTINFO(proflog, "Profiling started");
        fNDataProcessed = 0;
        Start();
        return;
    }

    void KTThroughputProfiler::Data(KTDataPtr data)
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
        double totalSeconds = TimeToSec(diffTime);
        KTPROG(proflog, "Throughput time: " << diffTime.tv_sec << " sec and " << diffTime.tv_nsec << " nsec (" << totalSeconds << " sec)");

        // Data production rate in bytes per second
        //double dataProductionRate = double(fEggHeader.GetNChannels()) * fEggHeader.GetAcquisitionRate() * double(fEggHeader.GetDataTypeSize());

        // Data throughput rate in bytes per second
        //double dataThroughputRate = 0.;
        //if (totalSeconds != 0)
        //    dataThroughputRate = double(fEggHeader.GetSliceSize() * fEggHeader.GetNChannels() * fNDataProcessed * fEggHeader.GetDataTypeSize()) / totalSeconds;

        //KTINFO(proflog, "Data production rate: " << dataProductionRate << " bytes per second");
        //KTINFO(proflog, "Data throughput rate: " << dataThroughputRate << " bytes per second");
        //KTPROG(proflog, "Analysis time factor: " << dataProductionRate / dataThroughputRate);

        return;
    }

    timespec KTThroughputProfiler::CurrentTime()
    {
        timespec ts;
        GetTimeCurrent(&ts);
        return ts;
    }

    timespec KTThroughputProfiler::Diff(timespec start, timespec end) const
    {
        timespec diff;
        TimeDiff(start, end, &diff);
        return diff;
    }

#ifdef __MACH__
    double KTThroughputProfiler::sTimebase = 0.0;
    uint64_t KTThroughputProfiler::sTimestart = 0;
#endif

    int KTThroughputProfiler::GetTimeCurrent(struct timespec* time)
    {
    #ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        if (! sTimestart)
        {
            mach_timebase_info_data_t tb = { .numer = 0, .denom = 1 };
            mach_timebase_info(&tb);
            sTimebase = tb.numer;
            sTimebase /= tb.denom;
            sTimestart = mach_absolute_time();
        }
        double diff = (mach_absolute_time() - sTimestart) * sTimebase;
        time->tv_sec = diff * MACNANO;
        time->tv_nsec = diff - (time->tv_sec * MACGIGA);
        return 0;
    #else
        return clock_gettime(CLOCK_PROCESS_CPUTIME_ID, time);
    #endif

    }

    double KTThroughputProfiler::TimeToSec(struct timespec time)
    {
        return (double)time.tv_sec + (double)time.tv_nsec / (double)NSEC_PER_SEC;
    }

    void KTThroughputProfiler::TimeDiff(struct timespec start, struct timespec end, struct timespec* diff)
    {
        if ((end.tv_nsec - start.tv_nsec < 0))
        {
            diff->tv_sec = end.tv_sec - start.tv_sec - 1;
            diff->tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
        }
        else
        {
            diff->tv_sec = end.tv_sec - start.tv_sec;
            diff->tv_nsec = end.tv_nsec - start.tv_nsec;
        }
        return;
    }

} /* namespace Nymph */
