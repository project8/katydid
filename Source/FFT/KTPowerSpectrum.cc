/*
 * KTPowerSpectrum.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTPowerSpectrum.hh"

#include "KTPhysicalArray.hh"

#include <cmath>
#include <iostream>

using std::string;

ClassImp(Katydid::KTPowerSpectrum);

namespace Katydid
{

    KTPowerSpectrum::KTPowerSpectrum() :
            KTComplexVector(),
            fBinWidth(1.)
    {
    }

    KTPowerSpectrum::KTPowerSpectrum(UInt_t nBins, Double_t binWidth) :
            KTComplexVector(nBins),
            fBinWidth(binWidth)
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
        fMagnitude.ResizeTo(freqSpect.GetSize());
        fPhase.ResizeTo(freqSpect.GetSize());

        fMagnitude = freqSpect.GetMagnitude();
        fMagnitude = fMagnitude.Sqr();
        fPhase = freqSpect.GetPhase();
        return;
    }

    void KTPowerSpectrum::TakeFrequencySpectrum(unsigned int nBins, const Double_t* real, const Double_t* imag)
    {
        fMagnitude.ResizeTo((Int_t)nBins);
        fPhase.ResizeTo((Int_t)nBins);

        for (Int_t iBin=0; iBin<nBins; iBin++)
        {
            fMagnitude[iBin] = real[iBin]*real[iBin] + imag[iBin]*imag[iBin];
            fPhase[iBin] = atan2(imag[iBin], real[iBin]);
        }
        return;
    }

    TH1D* KTPowerSpectrum::CreateMagnitudeHistogram() const
    {
        TH1D* hist = KTPowerSpectrum::CreateMagnitudeHistogram("hPowerSpectrum");
        return hist;
    }

    TH1D* KTPowerSpectrum::CreateMagnitudeHistogram(const string& name) const
    {
        Double_t freqMult = 1.e-6;
        unsigned int nBins = (unsigned int)fMagnitude.GetNoElements();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (Int_t)nBins, -0.5*fBinWidth*freqMult, fBinWidth * ((Double_t)nBins-0.5) * freqMult);
        for (unsigned int iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, fMagnitude[iBin]);
        }
        hist->SetXTitle("Frequency (MHz)");
        hist->SetYTitle("Power");
        return hist;
    }

    TH1D* KTPowerSpectrum::CreatePhaseHistogram() const
    {
        TH1D* hist = KTPowerSpectrum::CreatePhaseHistogram("hPowerSpectrumPhase");
        return hist;
    }

    TH1D* KTPowerSpectrum::CreatePhaseHistogram(const string& name) const
    {
        Double_t freqMult = 1.e-6;
        unsigned int nBins = fPhase.GetNoElements();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum Phase", (Int_t)nBins, -0.5*fBinWidth*freqMult, fBinWidth * ((Double_t)nBins-0.5) * freqMult);
        for (unsigned int iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, fPhase[iBin]);
        }
        hist->SetXTitle("Frequency (MHz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    KTPhysicalArray< 1, Double_t >* KTPowerSpectrum::CreateMagnitudePhysArr() const
    {
        Double_t freqMult = 1.e-6;
        size_t nBins = fMagnitude.GetNoElements();
        KTPhysicalArray< 1, Double_t >* physArray = new KTPhysicalArray< 1, Double_t >(nBins, -0.5*fBinWidth*freqMult, fBinWidth * ((Double_t)nBins-0.5) * freqMult);
        for (size_t bin=0; bin<nBins; bin++)
        {
            (*physArray)[bin] = fMagnitude(bin);
        }
        physArray->SetLabel("Frequency (MHz)");
        return physArray;
    }

    KTPhysicalArray< 1, Double_t >* KTPowerSpectrum::CreatePhasePhysArr() const
    {
        Double_t freqMult = 1.e-6;
        size_t nBins = fPhase.GetNoElements();
        KTPhysicalArray< 1, Double_t >* physArray = new KTPhysicalArray< 1, Double_t >(nBins, -0.5*fBinWidth*freqMult, fBinWidth * ((Double_t)nBins-0.5) * freqMult);
        for (size_t bin=0; bin<nBins; bin++)
        {
            (*physArray)[bin] = fPhase(bin);
        }
        physArray->SetLabel("Frequency (MHz)");
        return physArray;
    }

    TH1D* KTPowerSpectrum::CreatePowerDistributionHistogram() const
    {
        TH1D* hist = KTPowerSpectrum::CreatePowerDistributionHistogram("hPowerDistribution");
        return hist;
    }

    TH1D* KTPowerSpectrum::CreatePowerDistributionHistogram(const string& name) const
    {
        Double_t tMaxMag = -1.;
        Double_t tMinMag = 1.e9;
        unsigned int nBins = (unsigned int)fMagnitude.GetNoElements();
        for (unsigned int iBin=0; iBin<nBins; iBin++)
        {
            if (fMagnitude[iBin] < tMinMag) tMinMag = fMagnitude[iBin];
            if (fMagnitude[iBin] > tMaxMag) tMaxMag = fMagnitude[iBin];
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned int iBin=0; iBin<nBins; iBin++)
        {
            hist->Fill(fMagnitude[iBin]);
        }
        hist->SetXTitle("Power");
        return hist;
    }



} /* namespace Katydid */
