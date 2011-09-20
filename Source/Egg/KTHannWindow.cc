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
        if (fabs(time) <= fWidth/2.) return (*fWindowFunction)[TMath::Nint((time+fWidth/2.) / fBinWidth)];
        return 0.;
    }

    Double_t KTHannWindow::GetWeight(Int_t bin) const
    {
        if (bin >= 0 && bin < fWidthInBins) return (*fWindowFunction)[bin];
        return 0.;
    }

    void KTHannWindow::RebuildWindowFunction()
    {
        delete fWindowFunction;
        fWindowFunction = new TArrayD(fWidthInBins);
        Double_t twoPiOverNBinsMinus1 = TMath::TwoPi() / (Double_t)(fWidthInBins - 1);
        for (Int_t iBin=0; iBin<fWidthInBins; iBin++)
        {
            (*fWindowFunction)[iBin] = 0.5 * (1. - TMath::Cos((Double_t)iBin * twoPiOverNBinsMinus1));
        }
        return;
    }

} /* namespace Katydid */
