/*
 * KTEventLoop.cc
 *
 *  Created on: Apr 22, 2014
 *      Author: nsoblath
 */

#include "KTRootGuiLoop.hh"

#include "logger.hh"

#include "TSystem.h"

namespace Katydid
{
    LOGGER(evlog, "KTRootGuiLoop")

    KTRootGuiLoop::KTRootGuiLoop() :
#ifndef __CINT__
            KTEventLoop(),
#endif
            fStatus(kPreRun),
            fSleepTime(100000)
    {}

    KTRootGuiLoop::~KTRootGuiLoop()
    {
        if (IsActive())
        {
            Stop();
        }
    }

    void KTRootGuiLoop::Go()
    {
        if (fStatus >= kStopped || fStatus == kRunning) return;

        fStatus = kRunning;
        LDEBUG(evlog, "Starting event loop");
        while (fStatus == kRunning)
        //for (unsigned count = 0; count < 100 && fStatus == kRunning; ++count)
        {
            //LDEBUG(evlog, "count is " << count);
            usleep(fSleepTime);
            gSystem->ProcessEvents();
        }
        if (fStatus == kRunning) fStatus = kPaused;
        return;
    }

    void KTRootGuiLoop::Pause()
    {
        if (fStatus != kRunning) return;
        fStatus = kPaused;
        LDEBUG(evlog, "Pausing event loop");
    }

    void KTRootGuiLoop::Stop()
    {
        if (! IsActive()) return;
        fStatus = kStopped;
        LDEBUG(evlog, "Stopping event loop");
        return;
    }
/*
    void KTRootGuiLoop::Finish()
    {
        if (! IsActive()) return;
        fStatus = kComplete;
        LDEBUG(evlog, "loop complete");
        return;
    }
*/
    void KTRootGuiLoop::Reset()
    {
        fStatus = kPreRun;
        LDEBUG(evlog, "loop reset");
        return;
    }


} /* namespace Katydid */
