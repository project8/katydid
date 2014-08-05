/*
 * KTPowerSpectrum.cc
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#include "KTPowerSpectrum.hh"

#include "KTLogger.hh"

namespace Katydid
{

    KTLOGGER(pslog, "KTPowerSpectrum");

    KTPowerSpectrum::KTPowerSpectrum(size_t nBins, double rangeMin, double rangeMax) :
            KTPhysicalArray< 1, double >(nBins, rangeMin, rangeMax),
            fMode(kPower)
    {
        SetAxisLabel("Frequency (Hz)");
        SetDataLabel("Power (W)");
    }
    KTPowerSpectrum::KTPowerSpectrum(const KTPowerSpectrum& orig) :
            KTPhysicalArray< 1, double >(orig),
            fMode(orig.GetMode())
    {
    }

    KTPowerSpectrum::~KTPowerSpectrum()
    {
    }

    void KTPowerSpectrum::ConvertToPowerSpectrum()
    {
        if (fMode == kPower) return;

        KTDEBUG(pslog, "Converting to Power Spectrum");
        (*this) *= GetBinWidth();
        fMode = kPower;
        SetDataLabel("Power (W)");

        return;
    }

    void KTPowerSpectrum::ConvertToPowerSpectralDensity()
    {
        if (fMode == kPSD) return;

        KTDEBUG(pslog, "Converting to Power SpectralDensity");
        (*this) *= 1. / GetBinWidth();
        fMode = kPSD;
        SetDataLabel("Power Spectral Density (W/Hz)");

        return;
    }


    KTPowerSpectrum& KTPowerSpectrum::operator=(const KTPowerSpectrum& rhs)
    {
        KTPhysicalArray< 1, double >::operator=(rhs);
        fMode = rhs.fMode;
        return *this;
    }

    KTPowerSpectrum& KTPowerSpectrum::Scale(double scale)
    {
        (*this) *= scale;
        return *this;
    }

    double KTPowerSpectrum::GetResistance()
    {
        return fResistance;
    }

    const double KTPowerSpectrum::fResistance = 50.;

} /* namespace Katydid */

/*
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

    KTPowerSpectrum::KTPowerSpectrum(unsigned nBins, double binWidth) :
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

    void KTPowerSpectrum::TakeFrequencySpectrum(unsigned int nBins, const double* real, const double* imag)
    {
        fMagnitude.ResizeTo((int)nBins);
        fPhase.ResizeTo((int)nBins);

        for (int iBin=0; iBin<nBins; iBin++)
        {
            fMagnitude(iBin) = real(iBin)*real(iBin) + imag(iBin)*imag(iBin);
            fPhase(iBin) = atan2(imag(iBin), real(iBin));
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
        double freqMult = 1.e-6;
        unsigned int nBins = (unsigned int)fMagnitude.GetNoElements();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (int)nBins, -0.5*fBinWidth*freqMult, fBinWidth * ((double)nBins-0.5) * freqMult);
        for (unsigned int iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((int)iBin+1, fMagnitude(iBin));
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
        double freqMult = 1.e-6;
        unsigned int nBins = fPhase.GetNoElements();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum Phase", (int)nBins, -0.5*fBinWidth*freqMult, fBinWidth * ((double)nBins-0.5) * freqMult);
        for (unsigned int iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((int)iBin+1, fPhase(iBin));
        }
        hist->SetXTitle("Frequency (MHz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    KTPhysicalArray< 1, double >* KTPowerSpectrum::CreateMagnitudePhysArr() const
    {
        double freqMult = 1.e-6;
        size_t nBins = fMagnitude.GetNoElements();
        KTPhysicalArray< 1, double >* physArray = new KTPhysicalArray< 1, double >(nBins, -0.5*fBinWidth*freqMult, fBinWidth * ((double)nBins-0.5) * freqMult);
        for (size_t bin=0; bin<nBins; bin++)
        {
            (*physArray)[bin] = fMagnitude(bin);
        }
        physArray->SetLabel("Frequency (MHz)");
        return physArray;
    }

    KTPhysicalArray< 1, double >* KTPowerSpectrum::CreatePhasePhysArr() const
    {
        double freqMult = 1.e-6;
        size_t nBins = fPhase.GetNoElements();
        KTPhysicalArray< 1, double >* physArray = new KTPhysicalArray< 1, double >(nBins, -0.5*fBinWidth*freqMult, fBinWidth * ((double)nBins-0.5) * freqMult);
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
        double tMaxMag = -1.;
        double tMinMag = 1.e9;
        unsigned int nBins = (unsigned int)fMagnitude.GetNoElements();
        for (unsigned int iBin=0; iBin<nBins; iBin++)
        {
            if (fMagnitude(iBin) < tMinMag) tMinMag = fMagnitude(iBin);
            if (fMagnitude(iBin) > tMaxMag) tMaxMag = fMagnitude(iBin);
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned int iBin=0; iBin<nBins; iBin++)
        {
            hist->Fill(fMagnitude(iBin));
        }
        hist->SetXTitle("Power");
        return hist;
    }



}*/ /* namespace Katydid */
