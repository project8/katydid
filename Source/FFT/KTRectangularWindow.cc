/*
 * KTRectangularWindow.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTRectangularWindow.hh"

#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(windowlog, "katydid.fft");

    static KTNORegistrar< KTWindowFunction, KTRectangularWindow > sWFRectRegistrar("rectangular");

    KTRectangularWindow::KTRectangularWindow(const string& name) :
            KTWindowFunction(name),
            fBoxcarSize(1)
    {
    }

    KTRectangularWindow::~KTRectangularWindow()
    {
    }

    bool KTRectangularWindow::ConfigureWFSubclass(const KTPStoreNode* node)
    {
        SetBoxcarSize(node->GetData< unsigned >("boxcar-size", fBoxcarSize));

        KTDEBUG(windowlog, "Rectangular WF configured: boxcar size = " << fBoxcarSize);
        return true;
    }

    double KTRectangularWindow::GetWeight(double time) const
    {
        if (fabs(time - 0.5 * fLength) <= 0.5 * double(fBoxcarSize) * fBinWidth) return 1.;
        return 0.;
    }

    void KTRectangularWindow::RebuildWindowFunction()
    {
        fWindowFunction.resize(fSize);
        double halfBW = 0.5 * fBinWidth;
        for (unsigned iBin=0; iBin < fSize; iBin++)
        {
            fWindowFunction[iBin] = GetWeight(double(iBin) * fBinWidth + halfBW);
        }
        return;
    }


} /* namespace Katydid */
