/*
 * KTTimeFrequencyPolar.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTTimeFrequencyPolar.hh"

#include "KTLogger.hh"
#include "KTPowerSpectrum.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#endif

#include <sstream>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::stringstream;

namespace Katydid
{
    KTLOGGER(fslog, "katydid.fft");

    KTTimeFrequencyPolar::KTTimeFrequencyPolar() :
            KTPhysicalArray< 2, complexpolar< Double_t > >(),
            KTTimeFrequency()
    {
    }

    KTTimeFrequencyPolar::KTTimeFrequencyPolar(size_t nTimeBins, Double_t timeRangeMin, Double_t timeRangeMax, size_t nFreqBins, Double_t freqRangeMin, Double_t freqRangeMax) :
            KTPhysicalArray< 2, complexpolar< Double_t > >(nTimeBins, timeRangeMin, timeRangeMax, nFreqBins, freqRangeMin, freqRangeMax),
            KTTimeFrequency()
    {
    }
    KTTimeFrequencyPolar::KTTimeFrequencyPolar(const KTTimeFrequencyPolar& orig) :
            KTPhysicalArray< 2, complexpolar< Double_t > >(orig),
            KTTimeFrequency()
    {
    }

    KTTimeFrequencyPolar::~KTTimeFrequencyPolar()
    {
    }

    KTTimeFrequencyPolar& KTTimeFrequencyPolar::operator=(const KTTimeFrequencyPolar& rhs)
    {
        KTPhysicalArray< 2, complexpolar< Double_t > >::operator=(rhs);
        return *this;
    }

    KTTimeFrequencyPolar& KTTimeFrequencyPolar::CConjugate()
    {
        UInt_t nTimeBins = size(1);
        UInt_t nFreqBins = size(2);
#pragma omp parallel for
        for (UInt_t iTimeBin=0; iTimeBin<nTimeBins; iTimeBin++)
        {
            for (UInt_t iFreqBin=0; iFreqBin<nFreqBins; iFreqBin++)
            {
                (*this)(iTimeBin, iFreqBin).conj();
            }
        }
        return *this;
    }

/*
    KTPowerSpectrum* KTTimeFrequencyPolar::CreatePowerSpectrum() const
    {
        UInt_t nBins = size();
        KTPowerSpectrum* newPS = new KTPowerSpectrum(GetBinWidth(), GetRangeMin(), GetRangeMax());
        Double_t value;
        Double_t scaling = 1. / KTPowerSpectrum::GetResistance();
#pragma omp parallel for private(value)
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
           value = (*this)(iBin).abs();
           (*newPS)(iBin) = value * value * scaling;
        }
        return newPS;
    }

    void KTTimeFrequencyPolar::Print(unsigned startPrint, unsigned nToPrint) const
    {
        stringstream printStream;
        for (unsigned iBin = startPrint; iBin < startPrint + nToPrint; iBin++)
        {
            printStream << "Bin " << iBin << ";   x = " << GetBinCenter(iBin) <<
                    ";   y = " << (*this)(iBin) << "\n";
        }
        KTDEBUG(fslog, "\n" << printStream.str());
        return;
    }
*/

#ifdef ROOT_FOUND
    /*
    TH1D* KTTimeFrequencyPolar::CreateMagnitudeHistogram(const std::string& name) const
    {
        UInt_t nBins = size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Magnitude", (Int_t)nBins, GetRangeMin(), GetRangeMax());
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, (*this)(iBin).abs());
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Voltage (V)");
        return hist;
    }
    *//*
    TH1D* KTTimeFrequencyPolar::CreatePhaseHistogram(const std::string& name) const
    {
        UInt_t nBins = size();
        TH1D* hist = new TH1D(name.c_str(), "Frequency Spectrum: Phase", (Int_t)nBins, GetRangeMin(), GetRangeMax());
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            hist->SetBinContent((Int_t)iBin+1, (*this)(iBin).arg());
        }
        hist->SetXTitle("Frequency (Hz)");
        hist->SetYTitle("Phase");
        return hist;
    }
    */
    TH2D* KTTimeFrequencyPolar::CreatePowerHistogram(const std::string& name) const
    {
        UInt_t nTimeBins = size(1);
        UInt_t nFreqBins = size(2);
        TH2D* hist = new TH2D(name.c_str(), "Power Spectrum", (Int_t)nTimeBins, GetRangeMin(1), GetRangeMax(1), (Int_t)nFreqBins, GetRangeMin(2), GetRangeMax(2));
        Double_t value;
        Double_t scaling = 1. / KTPowerSpectrum::GetResistance();
        for (UInt_t iTimeBin=0; iTimeBin<nTimeBins; iTimeBin++)
        {
            for (UInt_t iFreqBin=0; iFreqBin<nFreqBins; iFreqBin++)
            {
                value = (*this)(iTimeBin, iFreqBin).abs();
                hist->SetBinContent((Int_t)iTimeBin + 1, (Int_t)iFreqBin + 1, value * value * scaling);
            }
        }
        hist->SetXTitle("Time (s)");
        hist->SetYTitle("Frequency (Hz)");
        hist->SetZTitle("Power (W)");
        return hist;
    }
    /*
    TH1D* KTTimeFrequencyPolar::CreatePowerDistributionHistogram(const std::string& name) const
    {
        Double_t tMaxMag = -1.;
        Double_t tMinMag = 1.e9;
        UInt_t nBins = size();
        Double_t value;
        Double_t scaling = 1. / KTPowerSpectrum::GetResistance();
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            value = (*this)(iBin).abs();
            value *= value * scaling;
            if (value < tMinMag) tMinMag = value;
            if (value > tMaxMag) tMaxMag = value;
        }
        if (tMinMag < 1. && tMaxMag > 1.) tMinMag = 0.;
        TH1D* hist = new TH1D(name.c_str(), "Power Distribution", 100, tMinMag*0.95, tMaxMag*1.05);
        for (UInt_t iBin=0; iBin<nBins; iBin++)
        {
            value = (*this)(iBin).abs();
            hist->Fill(value * value * scaling);
        }
        hist->SetXTitle("Power (W)");
        return hist;
    }
    */
#endif

} /* namespace Katydid */
