/*
 * KTDisplayWindow.cc
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#include "KTDisplayWindow.hh"

#include "TApplication.h"
#include "TGClient.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TGButton.h"
#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "TPluginManager.h"

ClassImp(Katydid::KTDisplayWindow);

namespace Katydid
{

    KTDisplayWindow::KTDisplayWindow(UInt_t width, UInt_t height) :
            fApp(NULL),
            fMain(NULL),
            fEcanvas(NULL),
            fDisplayCondition(),
            fMutex(),
            fStatus(kStopped)
    {
        pthread_mutex_init( &fMutex, NULL );
        pthread_cond_init( &fDisplayCondition, NULL );

        pthread_mutex_lock(&fMutex);

        fApp = new TApplication("DisplayWindow", 0, 0);
        //fApp->SetReturnFromRun(true);

        gPluginMgr->AddHandler("TVirtualHistPainter", "*", "THistPainter", "HistPainter", "THistPainter()");

        const TGWindow* parent = gClient->GetRoot();

        // Create a main frame
        fMain = new TGMainFrame(parent, width, height);

        // Create canvas widget
        fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain, width, height);
        fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 1));

        // Create a horizontal frame widget with buttons
        TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain, width, 40);

        TGTextButton *draw = new TGTextButton(hframe, "&Continue");
        draw->Connect("Clicked()", "Katydid::KTDisplayWindow", this, "Continue()");
        hframe->AddFrame(draw, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

        //TGTextButton *exit = new TGTextButton(hframe, "&Exit", "gApplication->Terminate(0)");
        //hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

        fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));

        // Set a name to the main frame
        fMain->SetWindowName("Data Display");

        // Map all subwindows of main frame
        fMain->MapSubwindows();

        // Initialize the layout algorithm
        fMain->Resize(fMain->GetDefaultSize());

        // Map main frame
        fMain->MapWindow();

        //pthread_mutex_unlock(&fMutex);
    }

    KTDisplayWindow::~KTDisplayWindow()
    {
        Stop();
        fMain->Cleanup();
        delete fMain;
        delete fApp;
    }

    void KTDisplayWindow::Run()
    {
        if (fStatus == kStopped)
        {
            fStatus = kRunning;
            pthread_mutex_unlock(&fMutex);
            fApp->Run();
            pthread_mutex_lock(&fMutex);
            fStatus = kStopped;
        }
        return;
    }

    void KTDisplayWindow::Stop()
    {
        if (fStatus == kRunning)
        {
            pthread_mutex_lock(&fMutex);
            fApp->Terminate();
            fStatus = kStopped;
        }
        return;
    }

    void KTDisplayWindow::Continue()
    {
        //fDisplayCondition.notify_all();
        pthread_mutex_lock(&fMutex);
        pthread_cond_signal(&fDisplayCondition);
        pthread_mutex_unlock(&fMutex);
        return;
    }

    void KTDisplayWindow::Draw(TH1* hist)
    {

        pthread_mutex_lock(&fMutex);
        hist->Draw();
        TCanvas *canvas = fEcanvas->GetCanvas();
        canvas->cd();
        canvas->Update();
        pthread_mutex_unlock(&fMutex);

        Wait();
        return;
    }

    void KTDisplayWindow::Draw(TH2* hist)
    {
        pthread_mutex_lock(&fMutex);
        hist->Draw();
        TCanvas *canvas = fEcanvas->GetCanvas();
        canvas->cd();
        canvas->Update();
        pthread_mutex_unlock(&fMutex);

        Wait();
        return;
    }

    void KTDisplayWindow::Wait()
    {
        //boost::unique_lock< boost::mutex > lock(fMutex);
        //fDisplayCondition.wait(lock);
        pthread_mutex_lock(&fMutex);
        pthread_cond_wait(&fDisplayCondition, &fMutex);
        pthread_mutex_unlock(&fMutex);

        return;
    }


} /* namespace Katydid */
