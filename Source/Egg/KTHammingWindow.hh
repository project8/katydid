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
            virtual Double_t GetWeight(Int_t bin) const;

        protected:
            virtual void RebuildWindowFunction();

    };

} /* namespace Katydid */
#endif /* KTHAMMINGWINDOW_HH_ */
