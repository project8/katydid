/*
 * KTHammingWindow.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTHammingWindow.hh"

#include "TArrayD.h"
#include "TMath.h"

#include <cmath>

namespace Katydid
{

    KTHammingWindow::KTHammingWindow() :
            KTEventWindowFunction()
    {
    }

    KTHammingWindow::KTHammingWindow(const KTEvent* event) :
            KTEventWindowFunction(event)
    {
    }

    KTHammingWindow::~KTHammingWindow()
    {
    }

    Double_t KTHammingWindow::GetWeight(Double_t time) const
    {
        if (fabs(time) <= fWidth/2.) return (*fWindowFunction)[TMath::Nint((time+fWidth/2.) / fBinWidth)];
        return 0.;
    }

    Double_t KTHammingWindow::GetWeight(Int_t bin) const
    {
        if (bin >= 0 && bin < fWidthInBins) return (*fWindowFunction)[bin];
        return 0.;
    }

    void KTHammingWindow::RebuildWindowFunction()
    {
        delete fWindowFunction;
        fWindowFunction = new TArrayD(fWidthInBins);
        Double_t twoPiOverNBinsMinus1 = TMath::TwoPi() / (Double_t)(fWidthInBins - 1);
        for (Int_t iBin=0; iBin<fWidthInBins; iBin++)
        {
            (*fWindowFunction)[iBin] = 0.54 - 0.46 * TMath::Cos((Double_t)iBin * twoPiOverNBinsMinus1);
        }
        return;
    }

} /* namespace Katydid */
