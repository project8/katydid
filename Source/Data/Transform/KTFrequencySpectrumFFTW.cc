/*
 * KTFrequencySpectrumFFTW.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumFFTW.hh"

#include "KTLogger.hh"
#include "KTPowerSpectrum.hh"
#include "KTFrequencySpectrumPolar.hh"

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
    KTLOGGER(fslog, "KTFrequencySpectrumFFTW");

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW() :
            KTPhysicalArray< 1, fftw_complex >(),
            KTFrequencySpectrum(),
            fNTimeBins(0),
            fPointCache()
    {
    }

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW(size_t nBins, double rangeMin, double rangeMax) :
            KTPhysicalArray< 1, fftw_complex >(nBins, rangeMin, rangeMax),
            KTFrequencySpectrum(),
            fNTimeBins(0),
            fPointCache()
    {
        //KTINFO(fslog, "number of bins: " << nBins << "   is size even? " << fIsSizeEven);
        //KTINFO(fslog, "neg freq offset: " << fNegFreqOffset);
    }

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW(const KTFrequencySpectrumFFTW& orig) :
            KTPhysicalArray< 1, fftw_complex >(orig),
            KTFrequencySpectrum(),
            fNTimeBins(orig.fNTimeBins),
            fPointCache()
    {
    }

    KTFrequencySpectrumFFTW::~KTFrequencySpectrumFFTW()
    {
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::operator=(const KTFrequencySpectrumFFTW& rhs)
    {
        KTPhysicalArray< 1, fftw_complex >::operator=(rhs);
        fNTimeBins = rhs.fNTimeBins;
        return *this;
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::CConjugate()
    {
        unsigned nBins = size();
#pragma omp parallel for
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            // order doesn't matter, so use fData[] to access values
            fData[iBin][1] = -fData[iBin][1];
        }
        return *this;
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::AnalyticAssociate()
    {
        // This is only valid if the original signal is Real only (not complex)
        // Note: the data storage array is accessed directly, so the FFTW data storage format is used.
        // Nyquist bin(s) and negative frequency bins are set to 0 (from size/2 to the end of the array)
        // DC bin stays as is (array position 0).
        // Positive frequency bins are multiplied by 2 (from array position 1 to size/2).
        unsigned nBins = size();
        unsigned nyquistPos = nBins / 2; // either the sole nyquist bin (if even # of bins) or the first of the two (if odd # of bins; bins are sequential in the array).
#pragma omp parallel for
        for (unsigned arrayPos=1; arrayPos<nyquistPos; arrayPos++)
        {
            fData[arrayPos][0] = fData[arrayPos][0] * 2.;
            fData[arrayPos][1] = fData[arrayPos][1] * 2.;
        }
#pragma omp parallel for
        for (unsigned arrayPos=nyquistPos; arrayPos<nBins; arrayPos++)
        {
            fData[arrayPos][0] = 0.;
            fData[arrayPos][1] = 0.;
        }
        return *this;
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::Scale(double scale)
    {
        unsigned nBins = size();
#pragma omp parallel for
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
            // order doesn't matter, so use fData[] to access values
            fData[iBin][0] = scale * fData[iBin][0];
            fData[iBin][1] = scale * fData[iBin][1];
        }
        return *this;
    }


    KTFrequencySpectrumPolar* KTFrequencySpectrumFFTW::CreateFrequencySpectrumPolar() const
    {
        unsigned nBins = size();
        KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar(nBins, GetRangeMin(), GetRangeMax());
        newFS->SetNTimeBins(fNTimeBins);
#pragma omp parallel for
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
           (*newFS)(iBin).set_rect((*this)(iBin)[0], (*this)(iBin)[1]);
        }
        return newFS;
    }

    KTPowerSpectrum* KTFrequencySpectrumFFTW::CreatePowerSpectrum() const
    {
        unsigned nBins = size();
        KTPowerSpectrum* newPS = new KTPowerSpectrum(nBins, GetRangeMin(), GetRangeMax());
        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)GetNTimeBins();
        double valueImag, valueReal;
#pragma omp parallel for private(valueReal, valueImag)
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
           valueReal = (*this)(iBin)[0];
           valueImag = (*this)(iBin)[1];
           (*newPS)(iBin) = (valueReal * valueReal + valueImag * valueImag) * scaling;
        }
        return newPS;
    }

    void KTFrequencySpectrumFFTW::Print(unsigned startPrint, unsigned nToPrint) const
    {
        stringstream printStream;
        for (unsigned iBin = startPrint; iBin < startPrint + nToPrint; ++iBin)
        {
            // order matters, so use (*this)() to access values
            printStream << "Bin " << iBin << ";   x = " << GetBinCenter(iBin) <<
                    ";   y = " << (*this)(iBin) << "\n";
        }
        KTDEBUG(fslog, "\n" << printStream.str());
        return;
    }

} /* namespace Katydid */
