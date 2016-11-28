/**
 @file KTDataDisplay.hh
 @brief Contains KTDataDisplay
 @details ROOT-based GUI Display
 @author: N. S. Oblath
 @date: Dec 12, 2013
 */

#ifndef KTDATADISPLAY_HH_
#define KTDATADISPLAY_HH_

#include "KTWriter.hh"

#include "KTDisplayWindow.hh"
#include "KTRootGuiLoop.hh"

#include "TCanvas.h"

namespace Katydid
{
    
    class KTDataDisplay;
    typedef Nymph::KTDerivedTypeWriter< KTDataDisplay > KTDataTypeDisplay;

  /*!
     @class KTDataDisplay
     @author N. S. Oblath

     @brief GUI Data Display

     @details

     Configuration name: "data-display"

     Available configuration values:

     Slots:
     - "aa":
     - "aa-dist":
     - "corr":
     - "corr-dist":
     - "corr-ts":
     - "corr-ts-dist":
     - "hough":
     - "gain-var":
     - "fs-fftw":
     - "fs-polar":
     - "fs-fftw-phase":
     - "fs-polar-phase":
     - "fs-fftw-power":
     - "fs-polar-power":
     - "fs-fftw-power-dist":
     - "fs-polar-power-dist":
     - "multi-fs-fftw":
     - "multi-fs-polar":
     - "norm-fs-fftw":
     - "norm-fs-polar":
     - "raw-ts":
     - "ts":
     - "ts-dist":
     - "wv":
     - "wv-2d":

    */

    class KTDataDisplay : public Nymph::KTWriterWithTypists< KTDataDisplay, KTDataTypeDisplay >
    {

        public:
            KTDataDisplay(const std::string& name = "data-display");
            virtual ~KTDataDisplay();

            bool Configure(const scarab::param_node* node);

            unsigned GetWidth() const;
            void SetWidth(unsigned width);

            unsigned GetHeight() const;
            void SetHeight(unsigned height);

            const std::string& GetDrawArgs() const;
            void SetDrawArgs(const std::string& args);

            const std::string& GetSaveAs() const;
            void SetSaveAs(const std::string& args);

            bool GetSaveAll() const;
            void SetSaveAll(bool saveAll);

            bool GetSaveAndCloseAll() const;
            void SetSaveAndCloseAll(bool saveAndCloseAll);

        private:
            unsigned fWidth;
            unsigned fHeight;
            std::string fDrawArgs;
            std::string fSaveAs;
            bool fSaveAll;
            bool fSaveAndCloseAll;

        public:
            bool Initialize();
            bool OpenWindow();

            bool IsReady();

            template< typename XDrawable >
            void Draw(XDrawable* drawable, std::string title = "untitled.pdf");

        private:
            KTDisplayWindow* fDisplayWindow;

            KTRootGuiLoop* fEventLoop;

    };

    inline unsigned KTDataDisplay::GetWidth() const
    {
        return fWidth;
    }
    inline void KTDataDisplay::SetWidth(unsigned width)
    {
        fWidth = width;
        return;
    }

    inline unsigned KTDataDisplay::GetHeight() const
    {
        return fHeight;
    }
    inline void KTDataDisplay::SetHeight(unsigned height)
    {
        fHeight = height;
        return;
    }

    inline const std::string& KTDataDisplay::GetDrawArgs() const
    {
        return fDrawArgs;
    }
    inline void KTDataDisplay::SetDrawArgs(const std::string& args)
    {
        fDrawArgs = args;
        return;
    }

    inline const std::string& KTDataDisplay::GetSaveAs() const
    {
        return fSaveAs;
    }
    inline void KTDataDisplay::SetSaveAs(const std::string& args)
    {
        fSaveAs = args;
        return;
    }

    inline bool KTDataDisplay::GetSaveAll() const
    {
        return fSaveAll;
    }
    inline void KTDataDisplay::SetSaveAll(bool saveAll)
    {
        fSaveAll = saveAll;
        return;
    }

    inline bool KTDataDisplay::GetSaveAndCloseAll() const
    {
        return fSaveAndCloseAll;
    }
    inline void KTDataDisplay::SetSaveAndCloseAll(bool saveAndCloseAll)
    {
        fSaveAndCloseAll = saveAndCloseAll;
        return;
    }

    template< typename XDrawable >
    void KTDataDisplay::Draw(XDrawable* drawable, std::string title)
    {
        if (! IsReady()) Initialize();

        if (! fEventLoop->IsActive()) return;

        fDisplayWindow->Draw(drawable, fDrawArgs);
        fDisplayWindow->GetCanvas()->Update();

        TString fn = TString::Format("%s_%s", title.c_str(), fSaveAs.c_str());

        if( fSaveAndCloseAll )
        {
            fDisplayWindow->GetCanvas()->SaveAs( fn );
            fDisplayWindow->Continue();
        }
        else
        {
            // this will allow the user to interact with the window
            // the thread will otherwise be "blocked" until the loop is exited (e.g. with the Continue or Cancel buttons)
            fEventLoop->Go();
            if( fSaveAll )
            {
                fDisplayWindow->GetCanvas()->SaveAs( fn );
            }
        }

        return;
    }


} /* namespace Katydid */
#endif /* KTDATADISPLAY_HH_ */
