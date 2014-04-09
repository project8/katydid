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
            fThreadID(0)
    {
    }

    KTDataDisplay::~KTDataDisplay()
    {
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
        // start thread
        fDisplayWindow = new KTDisplayWindow(fWidth, fHeight);
        pthread_create( &fThreadID, 0, &KTDataDisplay::ThreadSetupAndExecute, fDisplayWindow );

        return;
    }

    bool KTDataDisplay::OpenWindow()
    {
        if (fDisplayWindow == NULL)
        {
            Initialize();
        }

        return true;
    }

    void* KTDataDisplay::ThreadSetupAndExecute(void* voidthread)
    {
        pthread_cleanup_push(&KTDataDisplay::ThreadCleanup, voidthread);
        KTDisplayWindow* window = (KTDisplayWindow*) (voidthread);
        window->Run();
        pthread_cleanup_pop(0);
        pthread_exit(0);
    }

    void KTDataDisplay::ThreadCleanup(void* /*voidthread*/)
    {
        return;
    }



} /* namespace Katydid */
