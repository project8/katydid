/*
 * KTPowerSpectrum.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTPowerSpectrum.hh"

#include <cmath>
#include <iostream>

ClassImp(Katydid::KTPowerSpectrum);

namespace Katydid
{

    KTPowerSpectrum::KTPowerSpectrum() :
            KTComplexVector(),
            fBinWidth(1.)
    {
    }

    KTPowerSpectrum::KTPowerSpectrum(const KTPowerSpectrum& original) :
            KTComplexVector(original),
            fBinWidth(original.GetBinWidth())
    {
    }

    KTPowerSpectrum::~KTPowerSpectrum()
    {
    }

    void KTPowerSpectrum::TakeFrequencySpectrum(const KTComplexVector& freqSpect)
    {
        delete fMagnitude;
        delete fPhase;
        fMagnitude = new TVectorD(freqSpect.GetMagnitude()->Sqr());
        fPhase = new TVectorD(*(freqSpect.GetPhase()));
        return;
    }

    void KTPowerSpectrum::TakeFrequencySpectrum(Int_t nBins, const Double_t* real, const Double_t* imag)
    {
        delete fMagnitude;
        delete fPhase;
        fMagnitude = new TVectorD(nBins);
        fPhase = new TVectorD(nBins);

        for (Int_t iBin=0; iBin<nBins; iBin++)
        {
            (*fMagnitude)[iBin] = real[iBin]*real[iBin] + imag[iBin]*imag[iBin];
            (*fPhase)[iBin] = atan2(imag[iBin], real[iBin]);
        }
        return;
    }

    TH1D* KTPowerSpectrum::CreateMagnitudeHistogram() const
    {
        Int_t nBins = fMagnitude->GetNoElements();
        TH1D* hist = new TH1D("hPowerSpectrum", "Power Spectrum", nBins, -0.5*fBinWidth, fBinWidth * ((Double_t)nBins-0.5));
        for (Int_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent(iBin+1, (*fMagnitude)[iBin]);
        }
        hist->SetXTitle("Frequency");
        hist->SetYTitle("Power");
        return hist;
    }

    TH1D* KTPowerSpectrum::CreatePhaseHistogram() const
    {
        Int_t nBins = fPhase->GetNoElements();
        TH1D* hist = new TH1D("hPowerSpectrumPhase", "Power Spectrum Phase", nBins, -0.5*fBinWidth, fBinWidth * ((Double_t)nBins-0.5));
        for (Int_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent(iBin+1, (*fPhase)[iBin]);
        }
        hist->SetXTitle("Frequency");
        hist->SetYTitle("Phase");
        return hist;
    }

} /* namespace Katydid */
