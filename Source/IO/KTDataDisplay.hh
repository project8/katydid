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

namespace Katydid
{
    class KTDataDisplay;
    typedef KTDerivedTypeWriter< KTDataDisplay > KTDataTypeDisplay;

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
     - "ts":
     - "ts-dist":
     - "wv":
     - "wv-2d":

    */

    class KTDataDisplay : public KTWriterWithTypists< KTDataDisplay >
    {

        public:
            KTDataDisplay(const std::string& name = "data-display");
            virtual ~KTDataDisplay();

            Bool_t Configure(const KTParamNode* node);

            unsigned GetWidth() const;
            void SetWidth(unsigned width);

            unsigned GetHeight() const;
            void SetHeight(unsigned height);

        private:
            unsigned fWidth;
            unsigned fHeight;

        public:
            void Initialize();

            Bool_t OpenWindow();

            template< typename XDrawable >
            void Draw(XDrawable* drawable);

        private:
            static void* ThreadSetupAndExecute( void* voidthread );
            static void ThreadCleanup( void* /*voidthread*/ );

            KTDisplayWindow* fDisplayWindow;

            pthread_t fThreadID;


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

    template< typename XDrawable >
    void KTDataDisplay::Draw(XDrawable* drawable)
    {
        fDisplayWindow->Draw(drawable);
        return;
    }


} /* namespace Katydid */
#endif /* KTDATADISPLAY_HH_ */