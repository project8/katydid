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

namespace Katydid
{
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

} /* namespace Katydid */

#endif /* KTDISPLAYWINDOW_HH_ */
