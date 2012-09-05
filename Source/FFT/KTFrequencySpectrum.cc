/*
 * KTFrequencySpectrum.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrum.hh"

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

    KTFrequencySpectrum::KTFrequencySpectrum() :
            KTPhysicalArray< 1, complexpolar< Double_t > >()
    {
    }

    KTFrequencySpectrum::KTFrequencySpectrum(size_t nBins, Double_t rangeMin, Double_t rangeMax) :
            KTPhysicalArray< 1, complexpolar< Double_t > >(nBins, rangeMin, rangeMax)
    {
    }
    KTFrequencySpectrum::KTFrequencySpectrum(const KTFrequencySpectrum& orig) :
            KTPhysicalArray< 1, complexpolar< Double_t > >(orig)
    {
    }

    KTFrequencySpectrum::~KTFrequencySpectrum()
    {
    }

    KTFrequencySpectrum& KTFrequencySpectrum::operator=(const KTFrequencySpectrum& rhs)
    {
        KTPhysicalArray< 1, complexpolar< Double_t > >::operator=(rhs);
        return *this;
    }

    KTFrequencySpectrum& KTFrequencySpectrum::CConjugate()
    {
        UInt_t nBins = GetNBins();
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            (*this)[iBin].conj();
        }
        return *this;
    }


    KTPowerSpectrum* KTFrequencySpectrum::CreatePowerSpectrum() const
    {
        UInt_t nBins = GetNBins();
        KTPowerSpectrum* newPS = new KTPowerSpectrum(GetBinWidth(), GetRangeMin(), GetRangeMax());
        Double_t value;
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
           value = (*this)[iBin].abs();
           newPS[iBin] = value * value;
        }
        return newPS;
    }

    void KTFrequencySpectrum::Print(unsigned startPrint, unsigned nToPrint) const
    {
        stringstream printStream;
        for (unsigned iBin = startPrint; iBin < startPrint + nToPrint; iBin++)
        {
            printStream << "Bin " << iBin << ";   x = " << GetBinCenter(iBin) <<
                    ";   y = " << (*this)[iBin] << "\n";
        }
        KTDEBUG(fslog, "\n" << printStream.str());
        return;
    }


#ifdef ROOT_FOUND
    TH1D* KTFrequencySpectrum::CreateMagnitudeHistogram(const std::string& name) const
    {
        UInt_t nBins = GetNBins();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (Int_t)nBins, GetRangeMin(), GetRangeMax());
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, (*this)[iBin].abs());
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Power");
        return hist;
    }

    TH1D* KTFrequencySpectrum::CreatePhaseHistogram(const std::string& name) const
    {
        UInt_t nBins = GetNBins();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (Int_t)nBins, GetRangeMin(), GetRangeMax());
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, (*this)[iBin].arg());
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    TH1D* KTFrequencySpectrum::CreatePowerHistogram(const std::string& name) const
    {
        UInt_t nBins = GetNBins();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (Int_t)nBins, GetRangeMin(), GetRangeMax());
        Double_t value;
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            value = (*this)[iBin].abs();
            hist->SetBinContent((Int_t)iBin+1, value*value);
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Power");
        return hist;
    }

    TH1D* KTFrequencySpectrum::CreatePowerDistributionHistogram(const std::string& name) const
    {
        Double_t tMaxMag = -1.;
        Double_t tMinMag = 1.e9;
        UInt_t nBins = GetNBins();
        Double_t value;
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            value = (*this)[iBin].abs();
            value *= value;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            value = (*this)[iBin].abs();
            hist->Fill(value*value);
        }
        hist->SetXTitle("Power");
        return hist;
    }

#endif

} /* namespace Katydid */
