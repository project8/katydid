/*
 * KTHammingWindow.hh
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#ifndef KTHAMMINGWINDOW_HH_
#define KTHAMMINGWINDOW_HH_

#include "KTEventWindowFunction.hh"

namespace Katydid
{

    class KTHammingWindow : public KTEventWindowFunction
    {
        public:
            KTHammingWindow();
            KTHammingWindow(const KTEvent* event);
            virtual ~KTHammingWindow();

            virtual Double_t GetWeight(Double_t time) const;
            virtual Double_t GetWeight(UInt_t bin) const;

        protected:
            virtual void RebuildWindowFunction();

            ClassDef(KTHammingWindow, 2);

    };

} /* namespace Katydid */
#endif /* KTHAMMINGWINDOW_HH_ */
