/*
 * KTDataDisplay.cc
 *
 *  Created on: Dec 12, 2013
 *      Author: nsoblath
 */

#include "KTDataDisplay.hh"

#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTParam.hh"

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    KT_REGISTER_WRITER(KTDataDisplay, "data-display");
    KT_REGISTER_PROCESSOR(KTDataDisplay, "data-display");

    KTDataDisplay::KTDataDisplay(const std::string& name) :
            KTWriterWithTypists< KTDataDisplay >(name),
            fHeight(500),
            fWidth(700),
            fDisplayWindow(NULL),
            fEventLoop(NULL)
    {
    }

    KTDataDisplay::~KTDataDisplay()
    {
        delete fEventLoop;
    }

    bool KTDataDisplay::Configure(const KTParamNode* node)
    {
        if (node == NULL) return true;

        fHeight = node->GetValue< unsigned >("window-height", fHeight);
        fWidth = node->GetValue< unsigned >("window-width", fWidth);

        return true;
    }

    void KTDataDisplay::Initialize()
    {
        delete fEventLoop;
        fEventLoop = new KTRootGuiLoop();

        delete fDisplayWindow;
        fDisplayWindow = new KTDisplayWindow(fWidth, fHeight);
        fDisplayWindow->Connect("Cancel()", "Katydid::KTRootGuiLoop", fEventLoop, "StopLoop()");
        fDisplayWindow->Connect("Continue()", "Katydid::KTRootGuiLoop", fEventLoop, "PauseLoop()");

        return;
    }

    bool KTDataDisplay::IsReady()
    {
        if (fEventLoop == NULL || fDisplayWindow == NULL) return false;

        return true;
    }

} /* namespace Katydid */
