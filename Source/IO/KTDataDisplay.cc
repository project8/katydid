/*
 * KTDataDisplay.cc
 *
 *  Created on: Dec 12, 2013
 *      Author: nsoblath
 */

#include "KTDataDisplay.hh"

#include "KTLogger.hh"


namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    KT_REGISTER_WRITER(KTDataDisplay, "data-display");
    KT_REGISTER_PROCESSOR(KTDataDisplay, "data-display");

    KTDataDisplay::KTDataDisplay(const std::string& name) :
            KTWriterWithTypists< KTDataDisplay, KTDataTypeDisplay >(name),
            fHeight(500),
            fWidth(700),
            fDrawArgs(""),
            fDisplayWindow(NULL),
            fEventLoop(NULL)
    {
    }

    KTDataDisplay::~KTDataDisplay()
    {
        delete fEventLoop;
        delete fDisplayWindow;
    }

    bool KTDataDisplay::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return true;

        fHeight = node->get_value< unsigned >("window-height", fHeight);
        fWidth = node->get_value< unsigned >("window-width", fWidth);
        fDrawArgs = node->get_value< std::string >("draw-args", fDrawArgs);

        return true;
    }

    bool KTDataDisplay::Initialize()
    {
        if (gClient == NULL)
        {
            KTERROR(publog, "Unable to find the ROOT gClient; Did you start a TApplication? (either manually or via KTKatydidApp)");
            return false;
        }

        delete fEventLoop;
        fEventLoop = new KTRootGuiLoop();

        delete fDisplayWindow;
        fDisplayWindow = new KTDisplayWindow(fWidth, fHeight);
        fDisplayWindow->Connect("Cancel()", "Katydid::KTRootGuiLoop", fEventLoop, "Stop()");
        fDisplayWindow->Connect("Continue()", "Katydid::KTRootGuiLoop", fEventLoop, "Pause()");

        return true;
    }

    bool KTDataDisplay::OpenWindow()
    {
        if (! IsReady())
        {
            Initialize();
            return IsReady();
        }
        return true;
    }

    bool KTDataDisplay::IsReady()
    {
        if (fEventLoop == NULL || fDisplayWindow == NULL) return false;

        return true;
    }

} /* namespace Katydid */
