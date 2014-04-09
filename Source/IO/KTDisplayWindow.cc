/*
 * KTDisplayWindow.cc
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#include "KTDisplayWindow.hh"

/*
#include "TROOT.h"
#include "TApplication.h"
#include "TGTableLayout.h"
#include "TGFrame.h"
#include "TList.h"
#include "TGWindow.h"
#include "TRootEmbeddedCanvas.h"
#include "TEllipse.h"
#include "TBox.h"
#include "TArrow.h"
#include "TPolyLine.h"
#include "TGButton.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TGDoubleSlider.h"
#include "TGaxis.h"
#include "Riostream.h"
*/

//#include "TGApplication.h"
//#include "TGClient.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TGButton.h"
#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "TSystem.h"
//#include "TPluginManager.h"

ClassImp(KTDisplayWindow);

//namespace Katydid
//{

void KTDisplayWindow::ThreadCleanup(void* voidthread)
{
    return;
}

void* KTDisplayWindow::ThreadSetupAndExecute(void* voidthread)
{
    pthread_cleanup_push(&ThreadCleanup, voidthread);
    KTDisplayWindow* window = (KTDisplayWindow*)voidthread;
    window->DoRunLoop();
    pthread_cleanup_pop(0);
    pthread_exit(0);
}


    KTDisplayWindow::KTDisplayWindow(const TGWindow* win, UInt_t width, UInt_t height) :
            TGMainFrame(win, width, height),
            //fApp(NULL),
            //fMain(NULL),
            fEcanvas(NULL),
            fThreadID(0),
            //fCleanup(new TList()),
            fDisplayCondition(),
            fMutex(),
            fIsActive(false),
            fStatus(kStopped)
    {
        pthread_mutex_init( &fMutex, NULL );
        pthread_cond_init( &fDisplayCondition, NULL );

        pthread_mutex_lock(&fMutex);

        //fApp = new TGApplication("DisplayWindow", 0, 0);
        //fApp->SetReturnFromRun(true);

            /*
        const UInt_t max_size = 300;

        TGCompositeFrame *table = new TGCompositeFrame(this,400,400,kSunkenFrame);
        fCleanup->Add(table);

        TGTableLayout* tlo = new TGTableLayout(table, 9, 6);
        table->SetLayoutManager(tlo);
        TGLayoutHints* loh = new TGLayoutHints(kLHintsTop|kLHintsLeft|
                                               kLHintsExpandX|kLHintsExpandY);
        AddFrame(table,loh);

        TGTableLayoutHints* tloh;

        // The Canvas
        TRootEmbeddedCanvas* recanvas =
            new TRootEmbeddedCanvas("Shapes",table,max_size,max_size);
        tloh = new TGTableLayoutHints(2,5,2,6,
                                      kLHintsExpandX|kLHintsExpandY |
                                      kLHintsShrinkX|kLHintsShrinkY |
                                      kLHintsFillX|kLHintsFillY);
        table->AddFrame(recanvas,tloh);

        fCanvas = recanvas->GetCanvas();

        // The sliders
        fHSlider = new TGDoubleHSlider(table,max_size,kDoubleScaleBoth,100,
                                       kHorizontalFrame,GetDefaultFrameBackground(),
                                       kFALSE, kTRUE);
        tloh = new TGTableLayoutHints(2,5,0,1,
                                      kLHintsExpandX|kLHintsShrinkX|kLHintsFillX);
        table->AddFrame(fHSlider,tloh);
        fHSlider->Connect("PositionChanged()","KTDisplayWindow",this,"DoSlider()");

        fVSlider = new TGDoubleVSlider(table,max_size,kDoubleScaleBoth,200,
                                       kVerticalFrame,GetDefaultFrameBackground(),
                                       kTRUE,kTRUE);
        tloh = new TGTableLayoutHints(0,1,2,6,
                                      kLHintsExpandY|kLHintsShrinkY|kLHintsFillY);
        table->AddFrame(fVSlider,tloh);
        fVSlider->Connect("PositionChanged()","KTDisplayWindow",this,"DoSlider()");

        // The scales
        fHScaleCanvas =
            new TRootEmbeddedCanvas("H Scale",table,max_size,50);
        tloh = new TGTableLayoutHints(2,5,1,2,
                                      kLHintsExpandX|kLHintsShrinkX|kLHintsFillX);
        table->AddFrame(fHScaleCanvas,tloh);
        fHScaleCanvas->GetCanvas()->cd();

        fHScaleCanvas->GetCanvas()->Range(0,0,1,1);
        fHScale = new TGaxis(0.0,0.5, 1.0,0.5, 0.0,100.0, 510, "-");
        fHScale->SetLabelSize(0.4);
        fHScale->SetName("X Scale");
        fHScale->Draw();
        fHScaleCanvas->GetCanvas()->SetEditable(kFALSE);

        fVScaleCanvas =
            new TRootEmbeddedCanvas("V Scale",table,50,max_size);
        tloh = new TGTableLayoutHints(1,2,2,6,
                                      kLHintsExpandY|kLHintsShrinkY|kLHintsFillY);
        table->AddFrame(fVScaleCanvas,tloh);
        fVScaleCanvas->GetCanvas()->cd();

        fVScaleCanvas->GetCanvas()->Range(0,0,1,1);
        fVScale = new TGaxis(0.5,0.0, 0.50001,1.0, 0.0,100.0, 510, "-");
        fVScale->SetLabelSize(0.4);
        fVScale->SetName("Y Scale");
        fVScale->Draw();
        fVScaleCanvas->GetCanvas()->SetEditable(kFALSE);

        // Call this after scales/sliders are setup
        SetRange(0,0,100,100);

        // The shape buttons
        const char* shape_button_name[] = {
            "Ellipse", "Box", "Triangle", "Arrow", "Zoom Out", "Zoom In", "Close"
        };
        UInt_t ind;
        for (ind = 0; ind < 7; ++ind) {
           TGTextButton* button =
               new TGTextButton(table,shape_button_name[ind],ind);
           tloh = new TGTableLayoutHints(5,6,ind+2,ind+1+2,
                                         kLHintsExpandX|kLHintsExpandY |
                                         kLHintsShrinkX|kLHintsShrinkY |
                                         kLHintsFillX|kLHintsFillY);
           table->AddFrame(button,tloh);
           button->Resize(100,button->GetDefaultHeight());
           button->Connect("Clicked()","KTDisplayWindow",this,"DoButton()");
        }

        const char* ctrl_button_name[3][3] = {
           { "NW", "N", "NE" },
           { "W" , "C", "E" },
           { "SW", "S", "SE" }
        };
        UInt_t indx, indy;
        for (indx = 0; indx < 3; ++indx) for (indy = 0; indy < 3; ++indy) {
           TGTextButton* button = new TGTextButton(table,
                                                   ctrl_button_name[indy][indx],
                                                   (indx+1)*10 + indy +1);
           tloh = new TGTableLayoutHints(indx+2,indx+1+2, 6+indy,6+indy+1,
                                         kLHintsExpandX|kLHintsExpandY |
                                         kLHintsShrinkX|kLHintsShrinkY |
                                         kLHintsFillX|kLHintsFillY);
           table->AddFrame(button,tloh);
           button->Resize(100,button->GetDefaultHeight());
           button->Connect("Clicked()","KTDisplayWindow",this,"DoButton()");
        }

        table->Layout();

        // exit on close window
        Connect("CloseWindow()","TApplication",gApplication,"Terminate(=0)");

        MapSubwindows();
        Layout();
        MapWindow();
*/

        //gPluginMgr->AddHandler("TVirtualHistPainter", "*", "THistPainter", "HistPainter", "THistPainter()");

        //const TGWindow* parent = gClient->GetRoot();


        // Create a main frame
        //fMain = new TGMainFrame(parent, width, height);

        // Create canvas widget
        fEcanvas = new TRootEmbeddedCanvas("Ecanvas",this, width, 200);
        AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 1));

        // Create a horizontal frame widget with buttons
        TGHorizontalFrame *hframe = new TGHorizontalFrame(this, width, 40);

        TGTextButton* draw = new TGTextButton(hframe, "&Continue");
        draw->Connect("Clicked()", "KTDisplayWindow", this, "Continue()");
        hframe->AddFrame(draw, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

        //TGTextButton* cancel = new TGTextButton(hframe, "&Exit");
        //cancel->Connect("Clicked()", "KTDisplayWindow", this, "~KTDisplayWindow()");
        //hframe->AddFrame(cancel, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

        AddFrame(hframe, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));

        // exit on close window
        Connect("CloseWindow()", "KTDisplayWindow", this, "Cancel()");

        // Set a name to the main frame
        SetWindowName("Data Display");

        // Map all subwindows of main frame
        MapSubwindows();

        // Initialize the layout algorithm
        Resize(GetDefaultSize());

        // Map main frame
        MapWindow();

        pthread_mutex_unlock(&fMutex);
    }

    KTDisplayWindow::~KTDisplayWindow()
    {
        //fCanvas->Clear();
        //delete fCanvas;
        //fCleanup->Delete();
        //delete fCleanup;

        //Stop();
        Cleanup();
        //delete fMain;
        //delete fApp;
    }

    void KTDisplayWindow::DoRunLoop()
    {
        fIsActive = true;
        pthread_mutex_unlock(&fMutex);
        while(IsActive())
        {
            usleep(500);
            pthread_mutex_lock(&fMutex);
            gApplication->StartIdleing();
            gSystem->InnerLoop();
            gApplication->StopIdleing();
            pthread_mutex_unlock(&fMutex);
        }
        return;
    }

    void KTDisplayWindow::Run()
    {
        if (! fIsActive)
        {
            pthread_mutex_lock(&fMutex);
            pthread_create(&fThreadID, 0, &KTDisplayWindow::ThreadSetupAndExecute, this);
        }
        return;
    }

    void KTDisplayWindow::Stop()
    {
        if (IsActive())
        {
            pthread_mutex_lock(&fMutex);
            fIsActive = false;
            pthread_cond_broadcast(&fDisplayCondition);
            pthread_mutex_unlock(&fMutex);
        }
        return;
    }

    void KTDisplayWindow::Continue()
    {
        //fDisplayCondition.notify_all();
        pthread_mutex_lock(&fMutex);
        pthread_cond_broadcast(&fDisplayCondition);
        pthread_mutex_unlock(&fMutex);
        return;
    }

    void KTDisplayWindow::Cancel()
    {
        if (IsActive())
        {
            pthread_mutex_lock(&fMutex);
            fIsActive = false;
            pthread_cond_broadcast(&fDisplayCondition);
            pthread_mutex_unlock(&fMutex);
        }
        return;
    }

    void KTDisplayWindow::Draw(TH1* hist)
    {

        pthread_mutex_lock(&fMutex);
        if (fIsActive)
        {
            hist->Draw();
            TCanvas *canvas = fEcanvas->GetCanvas();
            canvas->cd();
            canvas->Update();
            Wait();
        }
        else
        {
            pthread_mutex_unlock(&fMutex);
        }
        return;
    }

    void KTDisplayWindow::Draw(TH2* hist)
    {
        pthread_mutex_lock(&fMutex);
        if (fIsActive)
        {
            hist->Draw();
            TCanvas *canvas = fEcanvas->GetCanvas();
            canvas->cd();
            canvas->Update();
            Wait();
        }
        else
        {
            pthread_mutex_unlock(&fMutex);
        }
        return;
    }
    /**/
    void KTDisplayWindow::Wait()
    {
        //boost::unique_lock< boost::mutex > lock(fMutex);
        //fDisplayCondition.wait(lock);
        pthread_cond_wait(&fDisplayCondition, &fMutex);

        return;
    }

    bool KTDisplayWindow::IsActive() const
    {
        bool status = false;
        pthread_mutex_lock(&fMutex);
        status = fIsActive;
        pthread_mutex_unlock(&fMutex);
        return status;
    }

    /**/
    /*
    void KTDisplayWindow::DoButton()
    {
       TGButton* button = (TGButton*)gTQSender;
       UInt_t id = button->WidgetId();

       double xmin, ymin, xmax, ymax;
       double xdiff, ydiff;
       fCanvas->GetRange(xmin,ymin,xmax,ymax);
       xdiff = xmax - xmin; ydiff = ymax - ymin;

       fCanvas->cd();

       switch (id) {
       case 0: {                   // Ellipse
          TEllipse* ellipse = new TEllipse(gRandom->Uniform(.8*xdiff)+.1*xdiff,
                                           gRandom->Uniform(.8*ydiff)+.1*ydiff,
                                           gRandom->Uniform(.1*xdiff),
                                           gRandom->Uniform(.1*ydiff));
          ellipse->SetFillColor(Color_t(gRandom->Uniform(10)));
          ellipse->Draw();
          fCanvas->Modified();
          fCanvas->Update();
          break;
       }
       case 1: {                   // Box
          TBox* box = new TBox(gRandom->Uniform(xdiff)+xmin,
                               gRandom->Uniform(ydiff)+ymin,
                               gRandom->Uniform(xdiff)+xmin,
                               gRandom->Uniform(ydiff)+ymin);
          box->SetFillColor(Color_t(gRandom->Uniform(10)));
          box->Draw();
          fCanvas->Modified();
          fCanvas->Update();
          break;
       }
       case 2: {                   // Triangle
          double x[4], y[4];
          x[0] = x[3] = gRandom->Uniform(xdiff)+xmin;
          y[0] = y[3] = gRandom->Uniform(ydiff)+ymin;
          x[1] = gRandom->Uniform(xdiff)+xmin;
          y[1] = gRandom->Uniform(ydiff)+ymin;
          x[2] = gRandom->Uniform(xdiff)+xmin;
          y[2] = gRandom->Uniform(ydiff)+ymin;
          TPolyLine* pl = new TPolyLine(4, x, y);
          pl->SetLineColor(Color_t(gRandom->Uniform(10)));
          pl->Draw();
          fCanvas->Modified();
          fCanvas->Update();
          break;
       }
       case 3: {                   // Arrow
          TArrow* arrow = new TArrow(gRandom->Uniform(xdiff)+xmin,
                                     gRandom->Uniform(ydiff)+ymin,
                                     gRandom->Uniform(xdiff)+xmin,
                                     gRandom->Uniform(ydiff)+ymin);
          arrow->SetLineColor(Color_t(gRandom->Uniform(10)));
          arrow->Draw();
          fCanvas->Modified();
          fCanvas->Update();
          break;
       }
       case 4:                     // Zoom out
          SetRange(xmin-xdiff*.1, ymin-ydiff*.1, xmax+xdiff*.1, ymax+ydiff*.1);
          break;
       case 5:                     // Zoom in
          SetRange(xmin+xdiff*.1, ymin+ydiff*.1, xmax-xdiff*.1, ymax-ydiff*.1);
          break;
       case 6:
          gApplication->Terminate(0);
          break;
       case 11:                    // nw
          SetRange(xmin-xdiff*.1, ymin+ydiff*.1, xmax-xdiff*.1, ymax+ydiff*.1);
          break;
       case 12:                    // w
          SetRange(xmin-xdiff*.1, ymin, xmax-xdiff*.1, ymax);
          break;
       case 13:                    // sw
          SetRange(xmin-xdiff*.1, ymin-ydiff*.1, xmax-xdiff*.1, ymax-ydiff*.1);
          break;
       case 23:                    // s
          SetRange(xmin, ymin-ydiff*.1, xmax, ymax-ydiff*.1);
          break;
       case 33:                    // se
          SetRange(xmin+xdiff*.1, ymin-ydiff*.1, xmax+xdiff*.1, ymax-ydiff*.1);
          break;
       case 32:                    // e
          SetRange(xmin+xdiff*.1, ymin, xmax+xdiff*.1, ymax);
          break;
       case 31:                    // ne
          SetRange(xmin+xdiff*.1, ymin+ydiff*.1, xmax+xdiff*.1, ymax+ydiff*.1);
          break;
       case 21:                    // n
          SetRange(xmin, ymin+ydiff*.1, xmax, ymax+ydiff*.1);
          break;
       case 22:                    // c
          SetRange(50.0-xdiff/2,50-ydiff/2,50+xdiff/2,50+ydiff/2);
          break;
       } // end switch(id)
    }

    void KTDisplayWindow::DoSlider()
    {
       float xmin, ymin, xmax, ymax;
       fHSlider->GetPosition(xmin,xmax);
       fVSlider->GetPosition(ymin,ymax);

       SetRange(xmin,ymin,xmax,ymax, kFALSE);
    }

    void KTDisplayWindow::SetRange(Float_t xmin, Float_t ymin, Float_t xmax, Float_t ymax,
                          Bool_t move_slider)
    {
       fCanvas->Range(xmin,ymin,xmax,ymax);

       //cerr << "x=[" << xmin << " - " << xmax << "], "
       //     << "y=[" << ymin << " - " << ymax << "]\n";

       fHScale->SetWmin(xmin);
       fHScale->SetWmax(xmax);
       fVScale->SetWmin(ymin);
       fVScale->SetWmax(ymax);

       // Set position of slider - don't if DoSlider called SetRange
       if (move_slider) {
          fHSlider->SetPosition(xmin,xmax);
          fVSlider->SetPosition(ymin,ymax);
       }

       fCanvas->Modified();
       fHScaleCanvas->GetCanvas()->Modified();
       fVScaleCanvas->GetCanvas()->Modified();
       fCanvas->Update();
       fHScaleCanvas->GetCanvas()->Update();
       fVScaleCanvas->GetCanvas()->Update();
       fClient->ForceRedraw();  //prevent GUI from being starved of redraws

    }
    */
//} /* namespace Katydid */
