/*
 * KTHannWindow.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTHannWindow.hh"

#include "TArrayD.h"
#include "TMath.h"

#include <cmath>

ClassImp(Katydid::KTHannWindow);

namespace Katydid
{

    KTHannWindow::KTHannWindow() :
            KTEventWindowFunction()
    {
    }

    KTHannWindow::KTHannWindow(const KTEvent* event) :
            KTEventWindowFunction(event)
    {
    }

    KTHannWindow::~KTHannWindow()
    {
    }

    Double_t KTHannWindow::GetWeight(Double_t time) const
    {
        if (fabs(time) <= fLength/2.) return (*fWindowFunction)[TMath::Nint((time+fLength/2.) / fBinWidth)];
        return 0.;
    }

    Double_t KTHannWindow::GetWeight(Int_t bin) const
    {
        if (bin >= 0 && bin < fSize) return (*fWindowFunction)[bin];
        return 0.;
    }

    void KTHannWindow::RebuildWindowFunction()
    {
        delete fWindowFunction;
        fWindowFunction = new TArrayD(fSize);
        Double_t twoPiOverNBinsMinus1 = TMath::TwoPi() / (Double_t)(fSize - 1);
        for (Int_t iBin=0; iBin<fSize; iBin++)
        {
            (*fWindowFunction)[iBin] = 0.5 * (1. - TMath::Cos((Double_t)iBin * twoPiOverNBinsMinus1));
        }
        return;
    }

} /* namespace Katydid */
