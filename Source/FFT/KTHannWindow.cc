/*
 * KTHannWindow.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTHannWindow.hh"

#include "KTFactory.hh"
#include "KTMath.hh"
#include "KTPStoreNode.hh"

#include <cmath>

namespace Katydid
{
    static KTDerivedRegistrar< KTEventWindowFunction, KTHannWindow > sEWFHannRegistrar("hann");

    KTHannWindow::KTHannWindow() :
            KTEventWindowFunction()
    {
    }

    KTHannWindow::KTHannWindow(const KTTimeSeriesData* tsData) :
            KTEventWindowFunction(tsData)
    {
    }

    KTHannWindow::~KTHannWindow()
    {
    }

    Bool_t KTHannWindow::ConfigureEventWindowFunctionSubclass(const KTPStoreNode* node)
    {
        return true;
    }

    Double_t KTHannWindow::GetWeight(Double_t time) const
    {
        if (fabs(time) <= fLength/2.) return fWindowFunction[KTMath::Nint((time+fLength/2.) / fBinWidth)];
        return 0.;
    }

    Double_t KTHannWindow::GetWeight(UInt_t bin) const
    {
        if (bin < fSize) return fWindowFunction[bin];
        return 0.;
    }

    void KTHannWindow::RebuildWindowFunction()
    {
        fWindowFunction.resize(fSize);
        Double_t twoPiOverNBinsMinus1 = KTMath::TwoPi() / (Double_t)(fSize - 1);
        for (Int_t iBin=0; iBin<fSize; iBin++)
        {
            fWindowFunction[iBin] = 0.5 * (1. - cos((Double_t)iBin * twoPiOverNBinsMinus1));
        }
        return;
    }

} /* namespace Katydid */
