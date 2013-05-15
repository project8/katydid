/*
 * KTHannWindow.cc
 *
 *  Created on: Sep 18, 2011
 *      Author: nsoblath
 */

#include "KTHannWindow.hh"

#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTMath.hh"
#include "KTPStoreNode.hh"

#include <cmath>

using std::string;

namespace Katydid
{
    static KTDerivedNORegistrar< KTWindowFunction, KTHannWindow > sWFHannRegistrar("hann");

    KTLOGGER(windowlog, "katydid.fft");

    KTHannWindow::KTHannWindow(const string& name) :
            KTWindowFunction(name)
    {
    }

    KTHannWindow::~KTHannWindow()
    {
    }

    Bool_t KTHannWindow::ConfigureWFSubclass(const KTPStoreNode* node)
    {
        KTDEBUG(windowlog, "Hann WF configured");
        return true;
    }

    Double_t KTHannWindow::GetWeight(Double_t time) const
    {
        return GetWeight(KTMath::Nint(time / fBinWidth));
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
