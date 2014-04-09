/*
 * KTDisplayWindow.hh
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#ifndef KTDISPLAYWINDOW_HH_
#define KTDISPLAYWINDOW_HH_

//#include <TQObject.h>
//#include <RQ_OBJECT.h>

#include "TGFrame.h"

//class TGApplication;
//class TGMainFrame;
class TRootEmbeddedCanvas;
class TH1;
class TH2;

//class TCanvas;
//class TGaxis;
//class TGDoubleSlider;
//class TList;
//class TRootEmbeddedCanvas;

//#include <boost/thread.hpp>
#ifndef __CINT__
#include <pthread.h>
#else
struct pthread_cond_t;
struct pthread_mutex_t;
typedef int pthread_t
#endif


//namespace Katydid
//{
    class KTDisplayWindow : public TGMainFrame
    {
        //RQ_OBJECT("KTDataDisplay")

        public:
            enum status
            {
                kRunning,
                kStopped
            };

        public:
            KTDisplayWindow(const TGWindow* win, UInt_t width = 200, UInt_t height = 200);
            virtual ~KTDisplayWindow();

            //void DoButton();
            //void DoSlider();
            //void SetRange(Float_t xmin, Float_t ymin, Float_t xmax, Float_t ymax, Bool_t move_slider = kTRUE);

            void Run();
            void Stop();

            void Continue();
            void Cancel();

            void Draw(TH1* hist);
            void Draw(TH2* hist);

            bool IsActive() const;
            status GetStatus() const;


        private:
            static void* ThreadSetupAndExecute(void* voidthread);
            static void ThreadCleanup(void* voidthread);

            void DoRunLoop();

            void Wait();

            //TGApplication* fApp;
            //TGMainFrame* fMain;
            TRootEmbeddedCanvas* fEcanvas;

            //TList* fCleanup;
            //TCanvas* fCanvas;
            //TRootEmbeddedCanvas* fHScaleCanvas;
            //TRootEmbeddedCanvas* fVScaleCanvas;
            //TGaxis* fHScale;
            //TGaxis* fVScale;
            //TGDoubleSlider* fHSlider;
            //TGDoubleSlider* fVSlider;

            pthread_t fThreadID;

            //boost::condition_variable fDisplayCondition;
            //boost::mutex fMutex;
            pthread_cond_t fDisplayCondition;
            pthread_mutex_t fMutex;

            bool fIsActive;

            status fStatus;

            ClassDef(KTDisplayWindow, 0);
    };

    inline KTDisplayWindow::status KTDisplayWindow::GetStatus() const
    {
        return fStatus;
    }

//} /* namespace Katydid */
#endif /* KTDISPLAYWINDOW_HH_ */
