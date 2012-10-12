/*
 * KTFrequencySpectrumFFTW.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumFFTW.hh"

#include "KTLogger.hh"
#include "KTPowerSpectrum.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <sstream>

using std::stringstream;

namespace Katydid
{
    KTLOGGER(fslog, "katydid.fft");

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW() :
            KTPhysicalArray< 1, fftw_complex >(),
            fNBinsPosFreqs(0)
    {
    }

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW(size_t totalBins, size_t nBinsPosFreqs, Double_t rangeMin, Double_t rangeMax) :
            KTPhysicalArray< 1, fftw_complex >(totalBins, rangeMin, rangeMax),
            fNBinsPosFreqs(nBinsPosFreqs)
    {
    }
    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW(const KTFrequencySpectrumFFTW& orig) :
            KTPhysicalArray< 1, fftw_complex >(orig),
            fNBinsPosFreqs(orig.fNBinsPosFreqs)
    {
    }

    KTFrequencySpectrumFFTW::~KTFrequencySpectrumFFTW()
    {
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::operator=(const KTFrequencySpectrumFFTW& rhs)
    {
        KTPhysicalArray< 1, fftw_complex >::operator=(rhs);
        fNBinsPosFreqs = rhs.fNBinsPosFreqs;
        return *this;
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::CConjugate()
    {
        UInt_t nBins = GetNBins();
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            (*this)(iBin)[1] = -fData[iBin][1];
        }
        return *this;
    }


    KTPowerSpectrum* KTFrequencySpectrumFFTW::CreatePowerSpectrum() const
    {
        KTPowerSpectrum* newPS = new KTPowerSpectrum(fNBinsPosFreqs, GetRangeMin(), GetRangeMax());
        Double_t value;
        Double_t scaling = 1. / KTPowerSpectrum::GetResistance();
        for (UInt_t iBin=0; iBin<fNBinsPosFreqs; iBin++)
        {
           value = fData[iBin][0] * fData[iBin][0] + fData[iBin][1] * fData[iBin][1];
           (*newPS)(iBin) = value * scaling;
        }
        return newPS;
    }

    void KTFrequencySpectrumFFTW::Print(unsigned startPrint, unsigned nToPrint) const
    {
        stringstream printStream;
        for (unsigned iBin = startPrint; iBin < startPrint + nToPrint; iBin++)
        {
            printStream << "Bin " << iBin << ";   x = " << GetBinCenter(iBin) <<
                    ";   y = " << fData[iBin] << "\n";
        }
        KTDEBUG(fslog, "\n" << printStream.str());
        return;
    }


#ifdef ROOT_FOUND
    TH1D* KTFrequencySpectrumFFTW::CreateMagnitudeHistogram(const std::string& name) const
    {
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", (Int_t)fNBinsPosFreqs, GetRangeMin(), GetRangeMax());
        for (UInt_t iBin=0; iBin<fNBinsPosFreqs; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, std::sqrt(fData[iBin][0] * fData[iBin][0] + fData[iBin][1] * fData[iBin][1]));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KTFrequencySpectrumFFTW::CreatePhaseHistogram(const std::string& name) const
    {
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Phase", (Int_t)fNBinsPosFreqs, GetRangeMin(), GetRangeMax());
        for (UInt_t iBin=0; iBin<fNBinsPosFreqs; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, std::atan2(fData[iBin][1], fData[iBin][0]));
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    TH1D* KTFrequencySpectrumFFTW::CreatePowerHistogram(const std::string& name) const
    {
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (Int_t)fNBinsPosFreqs, GetRangeMin(), GetRangeMax());
        Double_t value;
        Double_t scaling = 1. / KTPowerSpectrum::GetResistance();
        for (UInt_t iBin=0; iBin<fNBinsPosFreqs; iBin++)
        {
            value = fData[iBin][0] * fData[iBin][0] + fData[iBin][1] * fData[iBin][1];
            hist->SetBinContent((Int_t)iBin + 1, value * scaling);
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Power (W)");
        return hist;
    }

    TH1D* KTFrequencySpectrumFFTW::CreatePowerDistributionHistogram(const std::string& name) const
    {
        Double_t tMaxMag = -1.;
        Double_t tMinMag = 1.e9;
        Double_t value;
        Double_t scaling = 1. / KTPowerSpectrum::GetResistance();
        for (UInt_t iBin=0; iBin<fNBinsPosFreqs; iBin++)
        {
            value = (fData[iBin][0] * fData[iBin][0] + fData[iBin][1] * fData[iBin][1]) * scaling;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (UInt_t iBin=0; iBin<fNBinsPosFreqs; iBin++)
        {
            value = fData[iBin][0] * fData[iBin][0] + fData[iBin][1] * fData[iBin][1];
            hist->Fill(value * scaling);
        }
        hist->SetXTitle("Power (W)");
        return hist;
    }

#endif

} /* namespace Katydid */
