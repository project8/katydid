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

namespace Katydid
{
    static KTDerivedRegistrar< KTBundleWindowFunction, KTRectangularWindow > sEWFRectRegistrar("rectangular");

    KTRectangularWindow::KTRectangularWindow() :
            KTBundleWindowFunction()
    {
    }

    KTRectangularWindow::KTRectangularWindow(const KTTimeSeriesData* tsData) :
            KTBundleWindowFunction(tsData)
    {
    }

    KTRectangularWindow::~KTRectangularWindow()
    {
    }

    Bool_t KTRectangularWindow::ConfigureEventWindowFunctionSubclass(const KTPStoreNode* node)
    {
        return true;
    }

    Double_t KTRectangularWindow::GetWeight(Double_t time) const
    {
        if (fabs(time) <= fLength/2.) return 1.;
        return 0.;
    }

    Double_t KTRectangularWindow::GetWeight(UInt_t bin) const
    {
        if (bin < fSize) return 1.;
        return 0.;
    }

    void KTRectangularWindow::RebuildWindowFunction()
    {
        fWindowFunction.resize(fSize);
        for (Int_t iBin=0; iBin < fSize; iBin++)
        {
            fWindowFunction[iBin] = 1.;
        }
        return;
    }


} /* namespace Katydid */
