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

    const KTAxisProperties< 1 >& KTFrequencySpectrumPolar::GetAxis() const
    {
        return *this;
    }

    KTAxisProperties< 1 >& KTFrequencySpectrumPolar::GetAxis()
    {
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
        // This function creates a power spectrum that runs from the smallest to the largest absolute frequency.
        // It can handle frequency ranges that do or don't cross DC, and that are symmetric or asymmetric.
        // If the frequency range does not cross DC, the power spectrum range will not either.
        // If the frequency range crosses DC, the power spectrum range will run from DC to the maximum absolute frequency
        // In this case negative-frequency bins are added to positive-frequency bins.
        

        double maxFreq = std::max(fabs(GetRangeMin()), fabs(GetRangeMax()));
        double minFreq = -0.5 * GetBinWidth();
        unsigned nBins = (maxFreq - minFreq) / GetBinWidth();
        if (GetRangeMax() < 0. || GetRangeMin() > 0.)
        {
            minFreq = std::min(fabs(GetRangeMin()), fabs(GetRangeMax()));
            nBins = size();
        }

        KTPowerSpectrum* newPS = new KTPowerSpectrum(nBins, minFreq, maxFreq);
        for (unsigned iBin = 0; iBin < nBins; ++iBin) (*newPS)(iBin) = 0.;

        int dcBin = FindBin(0.);
        // default case: dcBin >= 0 && dcBin < size()
        int firstPosFreqBin = dcBin;
        int lastPosFreqBin = size();
        int firstNegFreqBin = 0;
        int lastNegFreqBin = dcBin;
        if (dcBin >= (int)size())
        {
            firstPosFreqBin = size(); // lastPosFreqBin = size();
            lastNegFreqBin = size(); // firstNegFreqBin = 0
        }
        else if (dcBin < 0)
        {
            firstPosFreqBin = 0; // lastPosFreqBin = size();
            firstNegFreqBin = dcBin; // lastNegFreqBin = dcBin;
        }

        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)GetNTimeBins();

        double value;
#pragma omp parallel for private(value)
        for (unsigned iBin = firstPosFreqBin; iBin < lastPosFreqBin; ++iBin)
        {
            value = (*this)(iBin).abs();
            (*newPS)(iBin) = value * value * scaling;
        }
#pragma omp parallel for private(value)
        for (unsigned iBin = firstNegFreqBin; iBin < lastNegFreqBin; ++iBin)
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

} /* namespace Katydid */
