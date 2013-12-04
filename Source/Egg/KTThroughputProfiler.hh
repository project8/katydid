/*
 * KTKTThroughputProfiler.hh
 *
 *  Created on: Nov 30, 2012
 *      Author: nsoblath
 *
 *      Portability for Mac OS X:
 *      Author: Jens Gustedt
 *      URL: http://stackoverflow.com/a/5167506
 */

#ifndef KTTHROUGHPUTPROFILER_HH_
#define KTTHROUGHPUTPROFILER_HH_

#include "KTProcessor.hh"

#include "KTEggHeader.hh"
#include "KTData.hh"

#include <boost/shared_ptr.hpp>

#include <string>

#include <time.h>


namespace Katydid
{
    class KTPStoreNode;

    /*!
     @class KTThroughputProfiler
     @author N. S. Oblath

     @brief Measures the speed of data processing

     @details

     A single timer is used to count the total processing time.

     The timer is started with a call to the Start function.
     The ProcessHeader function is provided as a convenient timer-starting slot called "start", since the Egg processor emits a matching signal before slice processing starts.
     If using the signal/slot interface, it's recommended to connect the "header" signal (e.g. from KTEggProcessor) to the "start" slot last (i.e. after any other processors have used the "header" signal).

     The timer is stopped with a call to the Stop function.
     The Finish function is provided as a convenient timer-stopping slot called "stop".
     If using the signal/slot interface, it's recommended to connect the "egg-done" signal (e.g. from KTEggProcessor) to the "stop" slot first (i.e. before any other processors have used the "egg-done" signal).

     Information printed:
     - Number of slices processed
     - Total throughput time -- time in seconds between calling the Start function and the Stop function
     - Data production rate -- bytes per second calculated assuming the slices were recorded in real-time
     - Data throughput rate -- bytes per second through the analysis
     - Analysis time factor -- ratio of the data throughput rate to the data production rate.  If this is 1, then the code analyzes the data at the same speed it was produced.

     Configuration name: "throughput-profiler"

     Available configuration values:

     Slots:
     - "start": void (const KTEggHeader*) -- Start the timer
     - "data": void (shared_ptr< KTData >) -- Increment the counter on the number of data slices
     - "stop": void () -- Stop the timer

    */
    class KTThroughputProfiler : public KTProcessor
    {

        public:
            KTThroughputProfiler(const std::string& name = "throughput-profiler");
            virtual ~KTThroughputProfiler();

            Bool_t Configure(const KTPStoreNode* node);

            void Start();
            void Stop();

            void ProcessHeader(const KTEggHeader* header);

            void ProcessData(boost::shared_ptr<KTData> data);

            void Finish();

            timespec Elapsed();

            Bool_t GetOutputFileFlag() const;
            void SetOutputFileFlag(Bool_t flag);

            const std::string& GetOutputFilename() const;
            void SetOutputFilename(const std::string& fname);

        private:
            timespec CurrentTime();
            timespec Diff(timespec start, timespec end) const;

            Bool_t fOutputFileFlag;
            std::string fOutputFilename;

            KTEggHeader fEggHeader;

            timespec fTimeStart;
            timespec fTimeEnd;

            UInt_t fNDataProcessed;

//#ifdef __MACH__
            double fMacTimebase;
            uint64_t fMacTimestart;
//#endif

    };

    inline Bool_t KTThroughputProfiler::GetOutputFileFlag() const
    {
        return fOutputFileFlag;
    }

    inline void KTThroughputProfiler::SetOutputFileFlag(Bool_t flag)
    {
        fOutputFileFlag = flag;
        return;
    }

    inline const std::string& KTThroughputProfiler::GetOutputFilename() const
    {
        return fOutputFilename;
    }

    inline void KTThroughputProfiler::SetOutputFilename(const std::string& fname)
    {
        fOutputFilename = fname;
        return;
    }

} /* namespace Katydid */
#endif /* KTTHROUGHPUTPROFILER_HH_ */
