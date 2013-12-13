/*
 * KTDataDisplay.cc
 *
 *  Created on: Dec 12, 2013
 *      Author: nsoblath
 */

#include "KTDataDisplay.hh"

#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTPStoreNode.hh"

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    static KTDerivedNORegistrar< KTWriter, KTDataDisplay > sDDWriterRegistrar("data-display");
    static KTDerivedNORegistrar< KTProcessor, KTDataDisplay > sDDProcRegistrar("data-display");

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

    Bool_t KTDataDisplay::Configure(const KTPStoreNode* node)
    {
        if (node == NULL) return true;

        fHeight = node->GetData< UInt_t >("window-height", fHeight);
        fWidth = node->GetData< UInt_t >("window-width", fWidth);

        return true;
    }

    void KTDataDisplay::Initialize()
    {
        // start thread
        fDisplayWindow = new KTDisplayWindow();
        pthread_create( &fThreadID, 0, &KTDataDisplay::ThreadSetupAndExecute, fDisplayWindow );

        return;
    }

    Bool_t KTDataDisplay::OpenWindow()
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
