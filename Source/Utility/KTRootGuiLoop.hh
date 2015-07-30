/*
 * KTEventLoop.hh
 *
 *  Created on: Apr 22, 2014
 *      Author: nsoblath
 */

#ifndef KTROOTGUILOOP_HH_
#define KTROOTGUILOOP_HH_

#ifndef __CINT__
#include "KTEventLoop.hh"
#else
namespace Nymph {} // this can be removed if something from Nymph is included outside the above ifndef statement
#endif

#include "RQ_OBJECT.h"

namespace Katydid
{
    using namespace ::Nymph;

    /*!
     @class KTRootGuiLoop
     @author N. S. Oblath

     @brief Custom event loop for ROOT GUIs

     @details

     Development note concerning ROOT issues: The CINT dictionary generator can't know that this class inherits
     from a non-ROOT class (KTEventLoop), but a dictionary must be generated for use of the signal/slot mechanism.
     Therefore references to KTEventLoop are contained in #ifndef blocks.
     */
    class KTRootGuiLoop
#ifndef __CINT__
            : public KTEventLoop
#endif
    {
            RQ_OBJECT("KTRootGuiLoop");
        public:
            enum Status
            {
                kPreRun = 0,
                kRunning = 10,
                kPaused = 20,
                kStopped = 100//,
                //kComplete = 200
            };

        public:
            KTRootGuiLoop();
            virtual ~KTRootGuiLoop();

            void Go();

            void Pause();
            void Stop();
            //void Finish();
            void Reset();

            bool IsActive() const;

            Status GetStatus() const;

            void SetSleepTime(unsigned time);
            unsigned GetSleepTime() const;

        private:
            Status fStatus;

            unsigned fSleepTime; /// microseconds

    };

    inline bool KTRootGuiLoop::IsActive() const
    {
        return fStatus <= kPaused;
    }

    inline KTRootGuiLoop::Status KTRootGuiLoop::GetStatus() const
    {
        return fStatus;
    }

    inline void KTRootGuiLoop::SetSleepTime(unsigned time)
    {
        if (fStatus == kRunning) return;
        fSleepTime = time;
        return;
    }

    inline unsigned KTRootGuiLoop::GetSleepTime() const
    {
        return fSleepTime;
    }

} /* namespace Katydid */

#endif /* KTROOTGUILOOP_HH_ */
