/*
 * KTEventLoop.cc
 *
 *  Created on: Apr 22, 2014
 *      Author: nsoblath
 */

#include "KTRootGuiLoop.hh"

#include "KTLogger.hh"

#include "TSystem.h"

namespace Katydid
{
    KTLOGGER(evlog, "KTRootGuiLoop")

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
        KTDEBUG(evlog, "Starting event loop");
        while (fStatus == kRunning)
        //for (unsigned count = 0; count < 100 && fStatus == kRunning; ++count)
        {
            //KTDEBUG(evlog, "count is " << count);
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
        KTDEBUG(evlog, "Pausing event loop");
    }

    void KTRootGuiLoop::Stop()
    {
        if (! IsActive()) return;
        fStatus = kStopped;
        KTDEBUG(evlog, "Stopping event loop");
        return;
    }
/*
    void KTRootGuiLoop::Finish()
    {
        if (! IsActive()) return;
        fStatus = kComplete;
        KTDEBUG(evlog, "loop complete");
        return;
    }
*/
    void KTRootGuiLoop::Reset()
    {
        fStatus = kPreRun;
        KTDEBUG(evlog, "loop reset");
        return;
    }


} /* namespace Katydid */
