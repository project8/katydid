/*
 * KTDisplayWindow.cc
 *
 *  Created on: Dec 13, 2013
 *      Author: nsoblath
 */

#include "KTDisplayWindow.hh"

//#include "TApplication.h"
#include "TGButton.h"
#include "TGFileDialog.h"
#include "TGFrame.h"
#include "TGMenu.h"
#include "TSystem.h"


ClassImp(Katydid::KTDisplayWindow);

namespace Katydid
{
    // Canvas menu command ids
    enum ERootCanvasCommands {
        kFileSaveAs,
        kFileCancel,

        kEditClear
    };

    static const char *gSaveAsTypes[] = { "PostScript",   "*.ps",
            "Encapsulated PostScript", "*.eps",
            "PDF",          "*.pdf",
            "SVG",          "*.svg",
            "TeX",          "*.tex",
            "GIF",          "*.gif",
            "ROOT macros",  "*.C",
            "ROOT files",   "*.root",
            "XML",          "*.xml",
            "PNG",          "*.png",
            "XPM",          "*.xpm",
            "JPEG",         "*.jpg",
            "TIFF",         "*.tiff",
            "XCF",          "*.xcf",
            "All files",    "*",
            0,              0 };

    KTDisplayWindow::KTDisplayWindow(unsigned width, unsigned height) :
            TGMainFrame(gClient->GetRoot(), width, height),
            fFileMenu(NULL),
            fEditMenu(NULL),
            fMenuBarLayout(NULL),
            fMenuBarItemLayout(NULL),
            fMenuBar(NULL),
            fEmbeddedCanvas(NULL),
            fButtonFrame(NULL),
            fContinueButton(NULL),
            fCancelButton(NULL),
            fCanceled(false)
    {
        SetCleanup(kDeepCleanup);
        Connect("CloseWindow()", "Katydid::KTDisplayWindow", this, "Cancel()");

        // Setup the GUI
        SetBackgroundColor(0xC0C0C0);

        // Add the menus
        fFileMenu = new TGPopupMenu(gClient->GetDefaultRoot());
        fFileMenu->AddEntry("Save &As...", kFileSaveAs);
        fFileMenu->AddSeparator();
        fFileMenu->AddEntry("&Cancel Display", kFileCancel);

        fEditMenu = new TGPopupMenu(gClient->GetDefaultRoot());
        fEditMenu->AddEntry("&Clear", kEditClear);

        fFileMenu->Associate(this);
        fEditMenu->Associate(this);

        TGLayoutHints* fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
        TGLayoutHints* fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

        TGMenuBar* fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
        fMenuBar->SetBackgroundColor(0xC0C0C0);
        fMenuBar->AddPopup("&File", fFileMenu, fMenuBarItemLayout);
        fMenuBar->AddPopup("&Edit", fEditMenu, fMenuBarItemLayout);

        AddFrame(fMenuBar, fMenuBarLayout);

        // Add the canvas
        fEmbeddedCanvas = new TRootEmbeddedCanvas("canvas", this, width, height-50);
        AddFrame(fEmbeddedCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 1));

        // Create a horizontal frame widget with buttons
        fButtonFrame = new TGHorizontalFrame(this, width, 40);
        fButtonFrame->SetBackgroundColor(0xC0C0C0);

        TGTextButton* continueButton = new TGTextButton(fButtonFrame, "&Continue");
        continueButton->Connect("Clicked()", "Katydid::KTDisplayWindow", this, "Continue()");
        fButtonFrame->AddFrame(continueButton, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

        TGTextButton* cancelButton = new TGTextButton(fButtonFrame, "C&ancel");
        cancelButton->Connect("Clicked()", "Katydid::KTDisplayWindow", this, "Cancel()");
        fButtonFrame->AddFrame(cancelButton, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

        AddFrame(fButtonFrame, new TGLayoutHints(kLHintsRight, 2, 50, 2, 2));

        // Set a name to the main frame
        SetWindowName("Katydid Data Display");

        // Map all subwindows of main frame
        MapSubwindows();

        // Initialize the layout algorithm
        Resize(GetDefaultSize());

        // Map main frame
        MapWindow();

    }

    KTDisplayWindow::~KTDisplayWindow()
    {
        Cleanup();
        // these three lines will get rid of the window upon deletion, but they'll kill the program.
        //DestroySubwindows();
        //DestroyWindow();
        //gApplication->Terminate(0);
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

    Bool_t KTDisplayWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
    {
        switch (GET_MSG(msg))
        {

            case kC_COMMAND:

                switch (GET_SUBMSG(msg))
                {

                    case kCM_BUTTON:
                    case kCM_MENU:

                        switch (parm1)
                        {

                            case kFileSaveAs:
                            {
                                TString workdir = gSystem->WorkingDirectory();
                                static TString dir(".");
                                static Int_t typeidx = 0;
                                static Bool_t overwr = kFALSE;
                                TGFileInfo fi;
                                fi.fFileTypes   = gSaveAsTypes;
                                fi.fIniDir      = StrDup(dir);
                                fi.fFileTypeIdx = typeidx;
                                fi.fOverwrite = overwr;
                                new TGFileDialog(fClient->GetDefaultRoot(), this, kFDSave, &fi);
                                gSystem->ChangeDirectory(workdir.Data());
                                if (! fi.fFilename) return kTRUE;
                                Bool_t  appendedType = kFALSE;
                                TString fn = fi.fFilename;
                                TString ft = fi.fFileTypes[fi.fFileTypeIdx+1];
                                dir     = fi.fIniDir;
                                typeidx = fi.fFileTypeIdx;
                                overwr  = fi.fOverwrite;
                                again:
                                if (fn.EndsWith(".root") ||
                                        fn.EndsWith(".ps")   ||
                                        fn.EndsWith(".eps")  ||
                                        fn.EndsWith(".pdf")  ||
                                        fn.EndsWith(".svg")  ||
                                        fn.EndsWith(".tex")  ||
                                        fn.EndsWith(".gif")  ||
                                        fn.EndsWith(".xml")  ||
                                        fn.EndsWith(".xpm")  ||
                                        fn.EndsWith(".jpg")  ||
                                        fn.EndsWith(".png")  ||
                                        fn.EndsWith(".xcf")  ||
                                        fn.EndsWith(".tiff"))
                                {
                                    fEmbeddedCanvas->GetCanvas()->SaveAs(fn);
                                }
                                else if (fn.EndsWith(".C"))
                                {
                                    fEmbeddedCanvas->GetCanvas()->SaveSource(fn);
                                }
                                else
                                {
                                    if (!appendedType) {
                                        if (ft.Index(".") != kNPOS) {
                                            fn += ft(ft.Index("."), ft.Length());
                                            appendedType = kTRUE;
                                            goto again;
                                        }
                                    }
                                    Warning("ProcessMessage", "file %s cannot be saved with this extension", fi.fFilename);
                                }
                            } // end of save-as block
                            break;
                            case kFileCancel:
                                Cancel();
                                break;

                                // Handle Edit menu items...
                            case kEditClear:
                                fEmbeddedCanvas->GetCanvas()->Clear();
                                fEmbeddedCanvas->GetCanvas()->Modified();
                                fEmbeddedCanvas->GetCanvas()->Update();
                                break;

                                /*
                                // Handle Option menu items...
                                case kOptionStatistics:
                                   if (gStyle->GetOptStat()) {
                                      gStyle->SetOptStat(0);
                                      delete gPad->FindObject("stats");
                                      fOptionMenu->UnCheckEntry(kOptionStatistics);
                                   } else {
                                      gStyle->SetOptStat(1);
                                      fOptionMenu->CheckEntry(kOptionStatistics);
                                   }
                                   gPad->Modified();
                                   fCanvas->Update();
                                   break;
                                case kOptionHistTitle:
                                   if (gStyle->GetOptTitle()) {
                                      gStyle->SetOptTitle(0);
                                      delete gPad->FindObject("title");
                                      fOptionMenu->UnCheckEntry(kOptionHistTitle);
                                   } else {
                                      gStyle->SetOptTitle(1);
                                      fOptionMenu->CheckEntry(kOptionHistTitle);
                                   }
                                   gPad->Modified();
                                   fCanvas->Update();
                                   break;
                                 */
                            default:
                                break;
                        } // end switch (parm1)
                            default:
                                break;
                } // end switch (GET_SUBMSG(msg))
                    default:
                        break;
        } // switch (GET_MSG(msg))
        return kTRUE;
    }

}
