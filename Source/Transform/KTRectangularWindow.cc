/*
 * KTRectangularWindow.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTRectangularWindow.hh"

#include "logger.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    LOGGER(windowlog, "RectangularWindow");

    KT_REGISTER_WINDOWFUNCTION(KTRectangularWindow, "rectangular")

    KTRectangularWindow::KTRectangularWindow(const string& name) :
            KTWindowFunction(name),
            fBoxcarSize(1)
    {
    }

    KTRectangularWindow::~KTRectangularWindow()
    {
    }

    bool KTRectangularWindow::ConfigureWFSubclass(const scarab::param_node* node)
    {
        SetBoxcarSize(node->get_value< unsigned >("boxcar-size", fBoxcarSize));

        LDEBUG(windowlog, "Rectangular WF configured: boxcar size = " << fBoxcarSize);
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
