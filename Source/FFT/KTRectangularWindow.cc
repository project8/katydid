/*
 * KTRectangularWindow.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTRectangularWindow.hh"

#include "KTFactory.hh"
#include "KTPStoreNode.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    static KTDerivedRegistrar< KTWindowFunction, KTRectangularWindow > sEWFRectRegistrar("rectangular");

    KTRectangularWindow::KTRectangularWindow(const string& name) :
            KTWindowFunction(name),
            fBoxcarSize(1)
    {
    }

    KTRectangularWindow::~KTRectangularWindow()
    {
    }

    Bool_t KTRectangularWindow::ConfigureWFSubclass(const KTPStoreNode* node)
    {
        SetBoxcarSize(node->GetData< UInt_t >("boxcar-size", fBoxcarSize));

        return true;
    }

    Double_t KTRectangularWindow::GetWeight(Double_t time) const
    {
        if (fabs(time - 0.5 * fLength) <= 0.5 * Double_t(fBoxcarSize) * fBinWidth) return 1.;
        return 0.;
    }

    void KTRectangularWindow::RebuildWindowFunction()
    {
        fWindowFunction.resize(fSize);
        Double_t halfBW = 0.5 * fBinWidth;
        for (UInt_t iBin=0; iBin < fSize; iBin++)
        {
            fWindowFunction[iBin] = GetWeight(Double_t(iBin) * fBinWidth + halfBW);
        }
        return;
    }


} /* namespace Katydid */
