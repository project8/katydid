/*
 * KTRectangularWindow.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTRectangularWindow.hh"

#include "TArrayD.h"
#include <cmath>

namespace Katydid
{

    KTRectangularWindow::KTRectangularWindow() :
            KTEventWindowFunction()
    {
    }

    KTRectangularWindow::KTRectangularWindow(const KTEvent* event) :
            KTEventWindowFunction(event)
    {
    }

    KTRectangularWindow::~KTRectangularWindow()
    {
    }

    Double_t KTRectangularWindow::GetWeight(Double_t time) const
    {
        if (fabs(time) <= fWidth/2.) return 1.;
        return 0.;
    }

    Double_t KTRectangularWindow::GetWeight(Int_t bin) const
    {
        if (bin >= 0 && bin < fWidthInBins) return 1.;
        return 0.;
    }

    void KTRectangularWindow::RebuildWindowFunction()
    {
        fWindowFunction = new TArrayD(fWidthInBins);
        for (Int_t iBin=0; iBin < fWidthInBins; iBin++)
        {
            (*fWindowFunction)[iBin] = 1.;
        }
        return;
    }


} /* namespace Katydid */
