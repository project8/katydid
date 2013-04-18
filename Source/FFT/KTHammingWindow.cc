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

using std::string;

namespace Katydid
{
    static KTDerivedRegistrar< KTWindowFunction, KTHammingWindow > sEWFHammRegistrar("hamming");

    KTHammingWindow::KTHammingWindow(const string& name) :
            KTWindowFunction(name)
    {
    }

    KTHammingWindow::~KTHammingWindow()
    {
    }

    Bool_t KTHammingWindow::ConfigureWFSubclass(const KTPStoreNode* node)
    {
        return true;
    }

    Double_t KTHammingWindow::GetWeight(Double_t time) const
    {
        return GetWeight(KTMath::Nint(time / fBinWidth));
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
