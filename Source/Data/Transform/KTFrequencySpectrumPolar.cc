/*
 * KTFrequencySpectrumPolar.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumPolar.hh"

#include "KTLogger.hh"
#include "KTPowerSpectrum.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <sstream>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::stringstream;

namespace Katydid
{
    KTLOGGER(fslog, "KTFrequencySpectrumPolar");

    KTFrequencySpectrumPolar::KTFrequencySpectrumPolar() :
            KTPhysicalArray< 1, complexpolar< double > >(),
            KTFrequencySpectrum(),
            fNTimeBins(0)
    {
    }

    KTFrequencySpectrumPolar::KTFrequencySpectrumPolar(size_t nBins, double rangeMin, double rangeMax) :
            KTPhysicalArray< 1, complexpolar< double > >(nBins, rangeMin, rangeMax),
            KTFrequencySpectrum(),
            fNTimeBins(0)
    {
    }
    KTFrequencySpectrumPolar::KTFrequencySpectrumPolar(const KTFrequencySpectrumPolar& orig) :
            KTPhysicalArray< 1, complexpolar< double > >(orig),
            KTFrequencySpectrum(),
            fNTimeBins(orig.fNTimeBins)
    {
    }

    KTFrequencySpectrumPolar::~KTFrequencySpectrumPolar()
    {
    }

    KTFrequencySpectrumPolar& KTFrequencySpectrumPolar::operator=(const KTFrequencySpectrumPolar& rhs)
    {
        KTPhysicalArray< 1, complexpolar< double > >::operator=(rhs);
        fNTimeBins = rhs.fNTimeBins;
        return *this;
    }

    KTFrequencySpectrumPolar& KTFrequencySpectrumPolar::CConjugate()
    {
        unsigned nBins = size();
#pragma omp parallel for
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            (*this)(iBin).conj();
        }
        return *this;
    }

    KTFrequencySpectrumPolar& KTFrequencySpectrumPolar::Scale(double scale)
    {
        (*this) *= scale;
        return *this;
    }

    KTPowerSpectrum* KTFrequencySpectrumPolar::CreatePowerSpectrum() const
    {
        unsigned nBins = size();
        KTPowerSpectrum* newPS = new KTPowerSpectrum(nBins, GetRangeMin(), GetRangeMax());
        double value;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)GetNTimeBins();
#pragma omp parallel for private(value)
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
           value = (*this)(iBin).abs();
           (*newPS)(iBin) = value * value * scaling;
        }
        return newPS;
    }

    void KTFrequencySpectrumPolar::Print(unsigned startPrint, unsigned nToPrint) const
    {
        stringstream printStream;
        for (unsigned iBin = startPrint; iBin < startPrint + nToPrint; ++iBin)
        {
            printStream << "Bin " << iBin << ";   x = " << GetBinCenter(iBin) <<
                    ";   y = " << (*this)(iBin) << "\n";
        }
        KTDEBUG(fslog, "\n" << printStream.str());
        return;
    }


#ifdef ROOT_FOUND
    TH1D* KTFrequencySpectrumPolar::CreateMagnitudeHistogram(const std::string& name) const
    {
        unsigned nBins = size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", (int)nBins, GetRangeMin(), GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*this)(iBin).abs());
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }

    TH1D* KTFrequencySpectrumPolar::CreatePhaseHistogram(const std::string& name) const
    {
        unsigned nBins = size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Phase", (int)nBins, GetRangeMin(), GetRangeMax());
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->SetBinContent((int)iBin+1, (*this)(iBin).arg());
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Phase");
        return hist;
    }

    TH1D* KTFrequencySpectrumPolar::CreatePowerHistogram(const std::string& name) const
    {
        unsigned nBins = size();
        TH1D* hist = new TH1D(name.c_str(), "Power Spectrum", (int)nBins, GetRangeMin(), GetRangeMax());
        double value;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)GetNTimeBins();
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            value = (*this)(iBin).abs();
            hist->SetBinContent((int)iBin + 1, value * value * scaling);
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Power (W)");
        return hist;
    }

    TH1D* KTFrequencySpectrumPolar::CreateMagnitudeDistributionHistogram(const std::string& name) const
    {
        double tMaxMag = -1.;
        double tMinMag = 1.e9;
        unsigned nBins = size();
        double value;
        // Skip the DC bin: start at bin 1
        for (unsigned iBin=1; iBin<nBins; ++iBin)
        {
            value = (*this)(iBin).abs();
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Magnitude Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            hist->Fill((*this)(iBin).abs());
        }
        hist->SetXTitle("Voltage (V)");
        return hist;
    }

    TH1D* KTFrequencySpectrumPolar::CreatePowerDistributionHistogram(const std::string& name) const
    {
        double tMaxMag = -1.;
        double tMinMag = 1.e9;
        unsigned nBins = size();
        double value;
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)GetNTimeBins();
        // Skip the DC bin: start at bin 1
        for (unsigned iBin=1; iBin<nBins; ++iBin)
        {
            value = (*this)(iBin).abs();
            value *= value * scaling;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            value = (*this)(iBin).abs();
            hist->Fill(value * value * scaling);
        }
        hist->SetXTitle("Power (W)");
        return hist;
    }

#endif

} /* namespace Katydid */
