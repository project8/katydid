/*
 * KTRectangularWindow.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTRectangularWindow.hh"

#include "KTFactory.hh"
#include "KTPStoreNode.hh"

#include "TArrayD.h"
#include <cmath>

ClassImp(Katydid::KTRectangularWindow);

namespace Katydid
{
    static KTDerivedRegistrar< KTEventWindowFunction, KTRectangularWindow > sEWFRectRegistrar("rectangular");

    KTRectangularWindow::KTRectangularWindow() :
            KTEventWindowFunction()
    {
    }

    KTRectangularWindow::KTRectangularWindow(const KTTimeSeriesData* tsData) :
            KTEventWindowFunction(tsData)
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
