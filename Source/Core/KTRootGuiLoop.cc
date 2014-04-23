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
            StopLoop();
        }
    }

    void KTRootGuiLoop::DoLoop()
    {
        if (fStatus >= kStopped || fStatus == kRunning) return;

        fStatus = kRunning;
        //while (true)
        for (unsigned count = 0; count < 100 && fStatus == kRunning; ++count)
        {
            KTDEBUG(evlog, "count is " << count);
            usleep(fSleepTime);
            gSystem->ProcessEvents();
        }
        if (fStatus == kRunning) fStatus = kPaused;
        return;
    }

    void KTRootGuiLoop::PauseLoop()
    {
        if (fStatus != kRunning) return;
        fStatus = kPaused;
        KTDEBUG(evlog, "Pausing event loop");
    }

    void KTRootGuiLoop::StopLoop()
    {
        if (! IsActive()) return;
        fStatus = kStopped;
        KTDEBUG(evlog, "Stopping event loop");
        return;
    }

    void KTRootGuiLoop::FinishLoop()
    {
        if (! IsActive()) return;
        fStatus = kComplete;
        KTDEBUG(evlog, "loop complete");
        return;
    }

    void KTRootGuiLoop::Reset()
    {
        fStatus = kPreRun;
        KTDEBUG(evlog, "loop reset");
        return;
    }


} /* namespace Katydid */
