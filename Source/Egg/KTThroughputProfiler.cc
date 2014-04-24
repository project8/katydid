/*
 * KTThroughputProfiler.cc
 *
 *  Created on: Nov 30, 2012
 *      Author: nsoblath
 */

#include "KTThroughputProfiler.hh"

#include "KTEggHeader.hh"
#include "KTParam.hh"

#include "MonarchTypes.hpp"

#include "thorax.hh"

#include <sstream>


using std::string;
using std::stringstream;
using std::vector;

namespace Katydid
{
    KTLOGGER(proflog, "KTThroughputProfiler");

    KT_REGISTER_PROCESSOR(KTThroughputProfiler, "throughput-profiler");

    KTThroughputProfiler::KTThroughputProfiler(const std::string& name) :
            KTProcessor(name),
            fOutputFileFlag(false),
            fOutputFilename("throughput.json"),
            fEggHeader(),
            fTimeStart(),
            fTimeEnd(),
            fNDataProcessed(0)
    {
        RegisterSlot("start", this, &KTThroughputProfiler::ProcessHeader);
        RegisterSlot("data", this, &KTThroughputProfiler::ProcessData);
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

    void KTThroughputProfiler::ProcessHeader(KTEggHeader* header)
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
        double totalSeconds = time_to_sec(diffTime);
        KTPROG(proflog, "Throughput time: " << diffTime.tv_sec << " sec and " << diffTime.tv_nsec << " nsec (" << totalSeconds << " sec)");

        // Data production rate in bytes per second
        double dataProductionRate = double(fEggHeader.GetNChannels()) * fEggHeader.GetAcquisitionRate() * double(fEggHeader.GetDataTypeSize());

        // Data throughput rate in bytes per second
        double dataThroughputRate = 0.;
        if (totalSeconds != 0)
            dataThroughputRate = double(fEggHeader.GetSliceSize() * fEggHeader.GetNChannels() * fNDataProcessed * fEggHeader.GetDataTypeSize()) / totalSeconds;

        KTINFO(proflog, "Data production rate: " << dataProductionRate << " bytes per second");
        KTINFO(proflog, "Data throughput rate: " << dataThroughputRate << " bytes per second");
        KTPROG(proflog, "Analysis time factor: " << dataProductionRate / dataThroughputRate);

        return;
    }

    timespec KTThroughputProfiler::CurrentTime()
    {
        timespec ts;
        get_time_current(&ts);
        return ts;
    }

    timespec KTThroughputProfiler::Diff(timespec start, timespec end) const
    {
        timespec diff;
        time_diff(start, end, &diff);
        return diff;
    }

} /* namespace Katydid */
