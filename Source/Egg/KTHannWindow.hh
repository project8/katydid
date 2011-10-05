/*
 * KTHannWindow.hh
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#ifndef KTHANNWINDOW_HH_
#define KTHANNWINDOW_HH_

#include "KTEventWindowFunction.hh"

namespace Katydid
{

    class KTHannWindow : public KTEventWindowFunction
    {
        public:
            KTHannWindow();
            KTHannWindow(const KTEvent* event);
            virtual ~KTHannWindow();

            virtual Double_t GetWeight(Double_t time) const;
            virtual Double_t GetWeight(Int_t bin) const;

        protected:
            virtual void RebuildWindowFunction();

            ClassDef(KTHannWindow, 1);

    };

} /* namespace Katydid */
#endif /* KTHANNWINDOW_HH_ */
