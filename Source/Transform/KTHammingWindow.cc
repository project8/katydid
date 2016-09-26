/*
 * KTHammingWindow.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTHammingWindow.hh"

#include "KTLogger.hh"
#include "KTMath.hh"
#include "param.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    KTLOGGER(windowlog, "KTHammingWindow");

    KT_REGISTER_WINDOWFUNCTION(KTHammingWindow, "hamming");

    KTHammingWindow::KTHammingWindow(const string& name) :
            KTWindowFunction(name)
    {
    }

    KTHammingWindow::~KTHammingWindow()
    {
    }

    bool KTHammingWindow::ConfigureWFSubclass(const scarab::param_node*)
    {
        KTDEBUG(windowlog, "Hamming WF configured");
        return true;
    }

    double KTHammingWindow::GetWeight(double time) const
    {
        return GetWeight(KTMath::Nint(time / fBinWidth));
    }

    void KTHammingWindow::RebuildWindowFunction()
    {
        fWindowFunction.resize(fSize);
        double twoPiOverNBinsMinus1 = KTMath::TwoPi() / (double)(fSize - 1);
        for (unsigned iBin=0; iBin<fSize; iBin++)
        {
            fWindowFunction[iBin] = 0.54 - 0.46 * cos((double)iBin * twoPiOverNBinsMinus1);
        }
        return;
    }

} /* namespace Katydid */
