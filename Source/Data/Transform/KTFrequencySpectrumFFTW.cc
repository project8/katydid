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

#include <sstream>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::stringstream;

namespace Katydid
{
    KTLOGGER(fslog, "KTFrequencySpectrumFFTW");

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW() :
            KTPhysicalArray< 1, std::complex<double> >(),
            KTFrequencySpectrum(),
            fIsArrayOrderFlipped(false),
            fIsSizeEven(true),
            fLeftOfCenterOffset(0),
            fCenterBin(0),
            fConstBinAccess(&KTFrequencySpectrumFFTW::AsIsBinAccess),
            fBinAccess(&KTFrequencySpectrumFFTW::AsIsBinAccess),
            fNTimeBins(0),
            fPointCache()
    {
    }

    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW(size_t nBins, double rangeMin, double rangeMax, bool arrayOrderIsFlipped) :
            KTPhysicalArray< 1, std::complex<double> >(nBins, rangeMin, rangeMax),
            KTFrequencySpectrum(),
            fIsArrayOrderFlipped(arrayOrderIsFlipped),
            fIsSizeEven(nBins%2 == 0),
            fLeftOfCenterOffset((nBins+1)/2),
            fCenterBin(nBins/2),
            fNTimeBins(0),
            fPointCache()
    {
        if (arrayOrderIsFlipped)
        {
            fConstBinAccess = &KTFrequencySpectrumFFTW::ReorderedBinAccess;
            fBinAccess = &KTFrequencySpectrumFFTW::ReorderedBinAccess;
        }
        else
        {
            fConstBinAccess = &KTFrequencySpectrumFFTW::AsIsBinAccess;
            fBinAccess = &KTFrequencySpectrumFFTW::AsIsBinAccess;
        }
        //KTINFO(fslog, "number of bins: " << nBins << "   is size even? " << fIsSizeEven);
        //KTINFO(fslog, "neg freq offset: " << fLeftOfCenterOffset);
    }

    // I have doubts about this function
    KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW(std::initializer_list<double> value, size_t nBins, double rangeMin, double rangeMax, bool arrayOrderIsFlipped) :
            KTFrequencySpectrumFFTW(nBins, rangeMin, rangeMax, arrayOrderIsFlipped)
    {
        for (unsigned index = 0; index < nBins; ++index)
        {
            std::copy(value.begin(), value.end(), &fData[index]);
        }
        //std::copy(value.begin(), value.end(), this->begin());
    }

    //copy constructor, destructor and copy assignment operator 
    //shouldn't be necessary. I think the default ones will do the right thing
    
    //~ KTFrequencySpectrumFFTW::KTFrequencySpectrumFFTW(const KTFrequencySpectrumFFTW& orig) :
            //~ KTPhysicalArray< 1, std::complex<double> >(orig),
            //~ KTFrequencySpectrum(),
            //~ fIsArrayOrderFlipped(orig.fIsArrayOrderFlipped),
            //~ fIsSizeEven(orig.fIsSizeEven),
            //~ fLeftOfCenterOffset(orig.fLeftOfCenterOffset),
            //~ fCenterBin(orig.fCenterBin),
            //~ fConstBinAccess(orig.fConstBinAccess),
            //~ fBinAccess(orig.fBinAccess),
            //~ fNTimeBins(orig.fNTimeBins),
            //~ fPointCache()
    //~ {
    //~ }

    //~ KTFrequencySpectrumFFTW::~KTFrequencySpectrumFFTW()
    //~ {
    //~ }
    
    //~ KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::operator=(const KTFrequencySpectrumFFTW& rhs)
    //~ {
        //~ KTPhysicalArray< 1, std::complex<double> >::operator=(rhs);
        //~ fIsArrayOrderFlipped = rhs.fIsArrayOrderFlipped;
        //~ fIsSizeEven = rhs.fIsSizeEven;
        //~ fLeftOfCenterOffset = rhs.fLeftOfCenterOffset;
        //~ fCenterBin = rhs.fCenterBin;
        //~ fConstBinAccess = rhs.fConstBinAccess;
        //~ fBinAccess = rhs.fBinAccess;
        //~ fNTimeBins = rhs.fNTimeBins;
        //~ return *this;
    //~ }

    const KTAxisProperties< 1 >& KTFrequencySpectrumFFTW::GetAxis() const
    {
        return *this;
    }

    KTAxisProperties< 1 >& KTFrequencySpectrumFFTW::GetAxis()
    {
        return *this;
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::CConjugate()
    {

        fData = conj(fData);
        
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
        
        fData.segment(1,nyquistPos-1) *= 2.0 ;
        fData.tail(nBins-nyquistPos) = std::complex<double> {0.0};

        return *this;
    }

    KTFrequencySpectrumFFTW& KTFrequencySpectrumFFTW::Scale(double scale)
    {
        
        fData *= scale;
        return *this;
    }

    // todo -> Replace KTFrequencySpectrumPolar 
    KTFrequencySpectrumPolar* KTFrequencySpectrumFFTW::CreateFrequencySpectrumPolar() const
    {
        unsigned nBins = size();
        KTFrequencySpectrumPolar* newFS = new KTFrequencySpectrumPolar(nBins, GetRangeMin(), GetRangeMax());
        newFS->SetNTimeBins(fNTimeBins);
#pragma omp parallel for
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
           (*newFS)(iBin).set_rect((*this)(iBin).real(), (*this)(iBin).imag());
        }
        return newFS;
    }

    KTPowerSpectrum* KTFrequencySpectrumFFTW::CreatePowerSpectrum() const
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
        
        // to replace after Eigen replaces all KTPhysicalArrays
        for (unsigned iBin = 0; iBin < nBins; ++iBin) (*newPS)(iBin) = 0.;
       // newPS->GetData().setZero(); //probably unnecessary, Eigen array should be zero-initialized

        int dcBin = FindBin(0.);
        // default case: dcBin >= 0 && dcBin < size()
        int firstPosFreqBin = dcBin;
        int lastPosFreqBin = size();
        int firstNegFreqBin = 0;
        int lastNegFreqBin = dcBin;
        if (dcBin >= (int)size())
        {
            firstPosFreqBin = size(); // lastPosFreqBin = size();
            lastNegFreqBin = size(); // firstNEgFreqBin = 0
        }
        else if (dcBin < 0)
        {
            firstPosFreqBin = 0; // lastPosFreqBin = size();
            firstNegFreqBin = dcBin; // lastNegFreqBin = dcBin;
        }
        //KTWARN( fslog, "firstPosFreqBin = " << firstPosFreqBin << "; lastPosFreqBin = " << lastPosFreqBin << "; firstNegFreqBin = " << firstNegFreqBin << "; lastNegFreqBin = " << lastNegFreqBin);

        double scaling = 1. / KTPowerSpectrum::GetResistance() / (double)GetNTimeBins();

        // to replace after Eigen replaces all KTPhysicalArrays
        //int nPosBins = lastPosFreqBin - firstPosFreqBin;
        //int nNegBins = lastNegFreqBin - firstNegFreqBin;
        //
        //newPS->GetData().segment(firstPosFreqBin, nPosBins) = 
        //            fData.segment(firstPosFreqBin, nPosBins).abs2() * scaling;
        //newPS->GetData().segment(firstNegFreqBin, nNegBins) = 
        //            fData.segment(firstNegFreqBin, nNegBins).abs2() * scaling;
                    
        // to replace after Eigen replaces all KTPhysicalArrays            
        double valueImag, valueReal;
#pragma omp parallel for private(valueReal, valueImag)
        for (unsigned iBin = firstPosFreqBin; iBin < lastPosFreqBin; ++iBin)
        {
            valueReal = (*this)(iBin).real();
            valueImag = (*this)(iBin).imag();
            (*newPS)(iBin) = (valueReal * valueReal + valueImag * valueImag) * scaling;
        }
#pragma omp parallel for private(valueReal, valueImag)
        for (unsigned iBin = firstNegFreqBin; iBin < lastNegFreqBin; ++iBin)
        {
            valueReal = (*this)(iBin).real();
            valueImag = (*this)(iBin).imag();
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
