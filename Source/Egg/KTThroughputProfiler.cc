/*
 * KTThroughputProfiler.cc
 *
 *  Created on: Nov 30, 2012
 *      Author: nsoblath
 */

#include "KTThroughputProfiler.hh"

#include "KTEggHeader.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include "MonarchTypes.hpp"

#include <sstream>

#ifdef __MACH__
#include <mach/mach_time.h>

#define MACNANO (+1.0E-9)
#define MACGIGA UINT64_C(1000000000)
#endif

using boost::shared_ptr;

using std::string;
using std::stringstream;
using std::vector;

namespace Katydid
{
    KTLOGGER(proflog, "katydid.egg");

    static KTDerivedRegistrar< KTProcessor, KTThroughputProfiler > sSimpleFFTRegistrar("throughput-profiler");

    KTThroughputProfiler::KTThroughputProfiler() :
            KTProcessor(),
            fOutputFileFlag(false),
            fOutputFilename("throughput.json"),
            fEggHeader(),
            fTimeStart(),
            fTimeEnd(),
            fNEventsProcessed(0),
            fMacTimebase(0.0),
            fMacTimestart(0)
    {
        fConfigName = "throughput-profiler";

        RegisterSlot("start", this, &KTThroughputProfiler::ProcessHeader, "void (const KTEggHeader*)");
        RegisterSlot("event", this, &KTThroughputProfiler::ProcessEvent, "void (shared_ptr<KTEvent>)");
        RegisterSlot("stop", this, &KTThroughputProfiler::Finish, "void ()");
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
        fNEventsProcessed = 0;
        Start();
        return;
    }

    void KTThroughputProfiler::ProcessEvent(shared_ptr<KTEvent> event)
    {
        fNEventsProcessed++;
        return;
    }

    void KTThroughputProfiler::Finish()
    {
        Stop();
        KTINFO(proflog, "Profiling stopped");
        timespec diffTime = Elapsed();
        KTINFO(proflog, fNEventsProcessed << " events processed");
        Double_t totalSeconds = Double_t(diffTime.tv_sec) + Double_t(diffTime.tv_nsec) * 1.e-9;
        KTINFO(proflog, "Throughput time: " << diffTime.tv_sec << " sec and " << diffTime.tv_nsec << " nsec (" << totalSeconds << " sec)");

        // Data production rate in bytes per second
        Double_t dataProductionRate = Double_t(fEggHeader.GetNChannels()) * fEggHeader.GetAcquisitionRate() * Double_t(sizeof(DataType));

        // Data throughput rate in bytes per second
        Double_t dataThroughputRate = 0.;
        if (totalSeconds != 0)
            dataThroughputRate = Double_t(fEggHeader.GetRecordSize() * fEggHeader.GetNChannels() * fNEventsProcessed * sizeof(DataType)) / totalSeconds;

        KTINFO(proflog, "Data production rate: " << dataProductionRate << " bytes per second");
        KTINFO(proflog, "Data throughput rate: " << dataThroughputRate << " bytes per second");
        KTINFO(proflog, "Analysis time factor: " << dataProductionRate / dataThroughputRate);

        return;
    }

    timespec KTThroughputProfiler::CurrentTime()
    {
        timespec ts;
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
        if (!fMacTimestart) {
            mach_timebase_info_data_t tb = { 0 };
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
