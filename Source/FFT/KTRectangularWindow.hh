/*
 * KTRectangularWindow.hh
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#ifndef KTRECTANGULARWINDOW_HH_
#define KTRECTANGULARWINDOW_HH_

#include "KTEventWindowFunction.hh"

namespace Katydid
{

    class KTRectangularWindow : public KTEventWindowFunction
    {
        public:
            KTRectangularWindow();
            KTRectangularWindow(const KTEvent* event);
            virtual ~KTRectangularWindow();

            virtual Double_t GetWeight(Double_t time) const;
            virtual Double_t GetWeight(Int_t bin) const;

        protected:
            virtual void RebuildWindowFunction();

            ClassDef(KTRectangularWindow, 1);

    };

} /* namespace Katydid */
#endif /* KTRECTANGULARWINDOW_HH_ */
