/*
 * KTDisplayWindow.cc
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#include "KTDisplayWindow.hh"

#include "TCanvas.h"
#include "TGButton.h"
#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"


ClassImp(Katydid::KTDisplayWindow);

namespace Katydid
{
    KTDisplayWindow::KTDisplayWindow(unsigned width, unsigned height) :
            TGMainFrame(gClient->GetRoot(), width, height),
            fEmbeddedCanvas(new TRootEmbeddedCanvas("canvas", this, width, height-50)),
            fCanceled(false)
    {
        SetCleanup(kDeepCleanup);
        Connect("CloseWindow()", "Katydid::KTDisplayWindow", this, "Cancel()");

        // Setup the GUI

        // Add a frame for the canvas
        AddFrame(fEmbeddedCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 1));

        // Create a horizontal frame widget with buttons
        TGHorizontalFrame *hframe = new TGHorizontalFrame(this, width, 40);

        TGTextButton* draw = new TGTextButton(hframe, "&Continue");
        draw->Connect("Clicked()", "Katydid::KTDisplayWindow", this, "Continue()");
        hframe->AddFrame(draw, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

        TGTextButton* cancel = new TGTextButton(hframe, "C&ancel");
        cancel->Connect("Clicked()", "Katydid::KTDisplayWindow", this, "Cancel()");
        hframe->AddFrame(cancel, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

        AddFrame(hframe, new TGLayoutHints(kLHintsRight, 2, 50, 2, 2));

        // Set a name to the main frame
        SetWindowName("Data Display");

        // Map all subwindows of main frame
        MapSubwindows();

        // Initialize the layout algorithm
        Resize(GetDefaultSize());

        // Map main frame
        MapWindow();

    }

    KTDisplayWindow::~KTDisplayWindow()
    {
    }

    void KTDisplayWindow::Continue()
    {
        Emit("Continue()");
        return;
    }

    void KTDisplayWindow::Cancel()
    {
        DontCallClose();
        if (! fCanceled )
        {
            Emit("Cancel()");
            fCanceled = true;
        }
        return;
    }

    bool KTDisplayWindow::IsCanceled() const
    {
        return fCanceled;
    }


    TCanvas* KTDisplayWindow::GetCanvas() const
    {
        return fEmbeddedCanvas->GetCanvas();
    }

}
