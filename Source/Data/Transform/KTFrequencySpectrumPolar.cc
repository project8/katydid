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

} /* namespace Katydid */
