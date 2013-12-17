/*
 * KTDisplayWindow.hh
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#ifndef KTDISPLAYWINDOW_HH_
#define KTDISPLAYWINDOW_HH_

#include <TQObject.h>
#include <RQ_OBJECT.h>

class TApplication;
class TGMainFrame;
class TH1;
class TH2;
class TRootEmbeddedCanvas;

//#include <boost/thread.hpp>
#ifndef __CINT__
#include <pthread.h>
#else
struct pthread_cond_t;
struct pthread_mutex_t;
#endif


namespace Katydid
{
    class KTDisplayWindow
    {
        RQ_OBJECT("KTDataDisplay")

        public:
            enum status
            {
                kRunning,
                kStopped
            };

        public:
            KTDisplayWindow(UInt_t width = 200, UInt_t height = 200);
            virtual ~KTDisplayWindow();

            void Run();
            void Stop();

            void Continue();

            void Draw(TH1* hist);
            void Draw(TH2* hist);

            status GetStatus() const;

        private:
            void Wait();

            TApplication* fApp;
            TGMainFrame* fMain;
            TRootEmbeddedCanvas* fEcanvas;

            //boost::condition_variable fDisplayCondition;
            //boost::mutex fMutex;
            pthread_cond_t fDisplayCondition;
            pthread_mutex_t fMutex;

            status fStatus;

            ClassDef(KTDisplayWindow, 0);
    };

    inline KTDisplayWindow::status KTDisplayWindow::GetStatus() const
    {
        return fStatus;
    }

} /* namespace Katydid */
#endif /* KTDISPLAYWINDOW_HH_ */
