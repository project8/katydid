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
#include "KTBundle.hh"

#include <boost/shared_ptr.hpp>

#include <string>

#include <time.h>


namespace Katydid
{
    class KTPStoreNode;

    class KTThroughputProfiler : public KTProcessor
    {

        public:
            KTThroughputProfiler();
            virtual ~KTThroughputProfiler();

            Bool_t Configure(const KTPStoreNode* node);

            void Start();
            void Stop();

            void ProcessHeader(const KTEggHeader* header);

            void ProcessBundle(boost::shared_ptr<KTBundle> bundle);

            void Finish();

            timespec Elapsed();

            const Bool_t GetOutputFileFlag() const;
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

            UInt_t fNBundlesProcessed;

//#ifdef __MACH__
            double fMacTimebase;
            uint64_t fMacTimestart;
//#endif

    };

    inline const Bool_t KTThroughputProfiler::GetOutputFileFlag() const
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
