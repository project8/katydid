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

    Double_t KTPowerSpectrum::GetPowerAtFrequency(Double_t freq)
    {
        return (*fMagnitude)[(Int_t)(freq / fBinWidth)];
    }

    Double_t KTPowerSpectrum::GetPhaseAtFrequency(Double_t freq)
    {
        return (*fPhase)[(Int_t)(freq / fBinWidth)];
    }


    TH1D* KTPowerSpectrum::CreateMagnitudeHistogram() const
    {
        Double_t freqMult = 1.e-6;
        Int_t nBins = fMagnitude->GetNoElements();
        TH1D* hist = new TH1D("hPowerSpectrum", "Power Spectrum", nBins, -0.5*fBinWidth*freqMult, fBinWidth * ((Double_t)nBins-0.5) * freqMult);
        for (Int_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent(iBin+1, (*fMagnitude)[iBin]);
        }
        hist->SetXTitle("Frequency (MHz)");
        hist->SetYTitle("Power");
        return hist;
    }

    TH1D* KTPowerSpectrum::CreatePhaseHistogram() const
    {
        Double_t freqMult = 1.e-6;
        Int_t nBins = fPhase->GetNoElements();
        TH1D* hist = new TH1D("hPowerSpectrumPhase", "Power Spectrum Phase", nBins, -0.5*fBinWidth*freqMult, fBinWidth * ((Double_t)nBins-0.5) * freqMult);
        for (Int_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent(iBin+1, (*fPhase)[iBin]);
        }
        hist->SetXTitle("Frequency (MHz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    TH1D* KTPowerSpectrum::CreatePowerDistributionHistogram() const
    {
        Double_t tMaxMag = -1.;
        Double_t tMinMag = 1.e9;
        for (Int_t iBin=0; iBin<fMagnitude->GetNoElements(); iBin++)
        {
            if ((*fMagnitude)[iBin] < tMinMag) tMinMag = (*fMagnitude)[iBin];
            if ((*fMagnitude)[iBin] > tMaxMag) tMaxMag = (*fMagnitude)[iBin];
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D("hPowerDistribution", "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (Int_t iBin=0; iBin<fMagnitude->GetNoElements(); iBin++)
        {
            hist->Fill((*fMagnitude)[iBin]);
        }
        hist->SetXTitle("Power");
        return hist;
    }

} /* namespace Katydid */
