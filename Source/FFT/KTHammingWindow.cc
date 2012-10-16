/*
 * KTHammingWindow.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTHammingWindow.hh"

#include "KTFactory.hh"
#include "KTMath.hh"
#include "KTPStoreNode.hh"

#include <cmath>

namespace Katydid
{
    static KTDerivedRegistrar< KTEventWindowFunction, KTHammingWindow > sEWFHammRegistrar("hamming");

    KTHammingWindow::KTHammingWindow() :
            KTEventWindowFunction()
    {
    }

    KTHammingWindow::KTHammingWindow(const KTTimeSeriesDataReal* tsData) :
            KTEventWindowFunction(tsData)
    {
    }

    KTHammingWindow::~KTHammingWindow()
    {
    }

    Bool_t KTHammingWindow::ConfigureEventWindowFunctionSubclass(const KTPStoreNode* node)
    {
        return true;
    }

    Double_t KTHammingWindow::GetWeight(Double_t time) const
    {
        if (fabs(time) <= fLength/2.) return fWindowFunction[KTMath::Nint((time+fLength/2.) / fBinWidth)];
        return 0.;
    }

    Double_t KTHammingWindow::GetWeight(UInt_t bin) const
    {
        if (bin < fSize) return fWindowFunction[bin];
        return 0.;
    }

    void KTHammingWindow::RebuildWindowFunction()
    {
        fWindowFunction.resize(fSize);
        Double_t twoPiOverNBinsMinus1 = KTMath::TwoPi() / (Double_t)(fSize - 1);
        for (Int_t iBin=0; iBin<fSize; iBin++)
        {
            fWindowFunction[iBin] = 0.54 - 0.46 * cos((Double_t)iBin * twoPiOverNBinsMinus1);
        }
        return;
    }

} /* namespace Katydid */
