/*
 * KTDisplayWindow.hh
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#ifndef KTDISPLAYWINDOW_HH_
#define KTDISPLAYWINDOW_HH_

#include "TGFrame.h"

class TCanvas;
class TRootEmbeddedCanvas;

//namespace Katydid
//{
    class KTDisplayWindow : public TGMainFrame
    {
        public:
            KTDisplayWindow(unsigned width = 200, unsigned height = 200);
            virtual ~KTDisplayWindow();

            TCanvas* GetCanvas() const;

            void Continue(); // *SIGNAL*

            void Cancel(); // *SIGNAL*
            bool IsCanceled() const;

        private:
            TRootEmbeddedCanvas* fEmbeddedCanvas;

            bool fCanceled;

            ClassDef(KTDisplayWindow, 0);
    };

//} /* namespace Katydid */

/*
#include "TApplication.h"
#include "TSystem.h"
#include <iostream>
using std::cout;
using std::endl;

class TestApplication
{
        //RQ_OBJECT("TestApplication");

    public:
        enum Status
        {
            kPreRun = 0,
            kRunning = 10,
            kStopped = 100,
            kComplete = 200
        };

    public:
        TestApplication() :
            fApp("TestApplication", 0, 0),
            fStatus(kPreRun)
        {}
        ~TestApplication()
        {}

        void EventLoop()
        {
            fStatus = kRunning;
            //while (true)
            for (unsigned count = 0; count < 100 && fStatus == kRunning; ++count)
            {
                cout << "count is " << count << endl;
                usleep(100000);
                gSystem->ProcessEvents();
            }
            return;
        }

        void StopLoop()
        {
            fStatus = kStopped;
            cout << "stopping event loop" << endl;
            return;
        }

        void FinishApplication()
        {
            fStatus = kComplete;
            cout << "loop complete" << endl;
            return;
        }

        Status GetStatus() const
        {
            return fStatus;
        }

    private:
        TApplication fApp;
        Status fStatus;
};

*/

#endif /* KTDISPLAYWINDOW_HH_ */
