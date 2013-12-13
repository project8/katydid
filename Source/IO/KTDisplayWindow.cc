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
//#include "TRandom.h"
#include "TGButton.h"
#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"

ClassImp(Katydid::KTDisplayWindow);

namespace Katydid
{

    KTDisplayWindow::KTDisplayWindow(UInt_t width, UInt_t height) :
            fApp(NULL),
            fMain(NULL),
            fEcanvas(NULL),
            fDisplayCondition(),
            fMutex()
    {
        pthread_mutex_init( &fMutex, NULL );
        pthread_cond_init( &fDisplayCondition, NULL );

        fApp = new TApplication("DisplayWindow", 0, 0);

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
    }

    KTDisplayWindow::~KTDisplayWindow()
    {
        fMain->Cleanup();
        delete fMain;
        delete fApp;
    }

    void KTDisplayWindow::Run()
    {
        fApp->Run();
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
/*
    void KTDisplayWindow::Draw()
    {
        // Draws function graphics in randomly choosen interval
        TF1 *f1 = new TF1("f1", "sin(x)/x", 0, gRandom->Rndm()*10);
        f1->SetLineWidth(3);
        f1->Draw();
        TCanvas *canvas = fEcanvas->GetCanvas();
        canvas->cd();
        canvas->Update();

        Wait();
        return;
    }
*/
    void KTDisplayWindow::Draw(TH1* hist)
    {
        hist->Draw();
        TCanvas *canvas = fEcanvas->GetCanvas();
        canvas->cd();
        canvas->Update();

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
