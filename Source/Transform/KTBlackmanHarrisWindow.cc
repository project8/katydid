/*
 * KTBlackmanHarrisWindow.cc
 *
 *  Created on: Apr 18, 2013
 *      Author: nsoblath
 */

#include "KTBlackmanHarrisWindow.hh"

#include "logger.hh"
#include "KTMath.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    LOGGER(windowlog, "KTBlackmanHarrisWindow");

    KT_REGISTER_WINDOWFUNCTION(KTBlackmanHarrisWindow, "blackman-harris")

    KTBlackmanHarrisWindow::KTBlackmanHarrisWindow(const string& name) :
            KTWindowFunction(name)
    {
    }

    KTBlackmanHarrisWindow::~KTBlackmanHarrisWindow()
    {
    }

    bool KTBlackmanHarrisWindow::ConfigureWFSubclass(const scarab::param_node*)
    {
        LDEBUG(windowlog, "Blackman-Harris WF configured");
        return true;
    }

    double KTBlackmanHarrisWindow::GetWeight(double time) const
    {
        return GetWeight(KTMath::Nint(time / fBinWidth));
    }

    void KTBlackmanHarrisWindow::RebuildWindowFunction()
    {
        fWindowFunction.resize(fSize);
        double twoPiOverNBinsMinus1 = KTMath::TwoPi() / (double)(fSize - 1);
        for (unsigned iBin=0; iBin<fSize; iBin++)
        {
            fWindowFunction[iBin] = 0.35875 - 0.48829 * cos(double(iBin) * twoPiOverNBinsMinus1) +
                    0.14128 * cos(double(2 * iBin) * twoPiOverNBinsMinus1) - 0.01168 * cos(double(3 * iBin) * twoPiOverNBinsMinus1);
        }
        return;
    }

} /* namespace Katydid */
