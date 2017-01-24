/*
 * KTDisplayWindow.hh
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#ifndef KTDISPLAYWINDOW_HH_
#define KTDISPLAYWINDOW_HH_

#include "TGFrame.h"

#include "TCanvas.h"
#include "TRootEmbeddedCanvas.h"

class TCanvas;
class TGPopupMenu;
class TGLayoutHints;
class TGMenuBar;
class TGHorizontalFrame;
class TGTextButton;

namespace Katydid
{
    class KTDisplayWindow : public TGMainFrame
    {
        public:
            KTDisplayWindow(unsigned width = 200, unsigned height = 200);
            virtual ~KTDisplayWindow();

            template< class XDrawable >
            void Draw(XDrawable* drawable, std::string args);

            TCanvas* GetCanvas() const;

            void Continue(); // *SIGNAL*

            void Cancel(); // *SIGNAL*
            bool IsCanceled() const;

            // process messages from user interactions; overridden from TGMainFrame
            Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

        private:
            TGPopupMenu* fFileMenu;
            TGPopupMenu* fEditMenu;
            TGLayoutHints* fMenuBarLayout;
            TGLayoutHints* fMenuBarItemLayout;
            TGMenuBar* fMenuBar;

            TRootEmbeddedCanvas* fEmbeddedCanvas;

            TGHorizontalFrame *fButtonFrame;
            TGTextButton* fContinueButton;
            TGTextButton* fCancelButton;

            bool fCanceled;

            ClassDef(KTDisplayWindow, 0);
    };

    template< class XDrawable >
    void KTDisplayWindow::Draw(XDrawable* drawable, std::string args)
    {
        fEmbeddedCanvas->GetCanvas()->cd();
        drawable->Draw(args.c_str());
        return;
    }


} /* namespace Katydid */

#endif /* KTDISPLAYWINDOW_HH_ */
