/*
 * KTChirpSpaceFFT.cc
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#include "KTChirpSpaceFFT.hh"

#include "KTLogger.hh"
#include "KTPowerSpectrum.hh"
//#include "KTFrequencySpecrumPolar.hh"

#include <sstream>

#ifdef USE_OPENMP
#include <omp.h>
#endif

using std::stringstream;

namespace Katydid
{
    KTLOGGER(fslog, "KTChirpSpaceFFT");

    KTChirpSpaceFFT::KTChirpSpaceFFT() :
            KTPhysicalArray< 2, std::complex<double> >(),
            KTChirpSpace(),
            fIsArrayOrderFlipped(false),
            fIsSizeEven(true),
            fLeftOfCenterOffset(0),
            fCenterBin(0),
            fConstBinAccess(&KTChirpSpaceFFT::AsIsBinAccess),
            fBinAccess(&KTChirpSpaceFFT::AsIsBinAccess),
            fNTimeBins(0),
            fPointCache()
    {
    }

    KTChirpSpaceFFT::KTChirpSpaceFFT(size_t nBins0, double rangeMin0, double rangeMax0, size_t nBins1, double rangeMin1, double rangeMax1, bool arrayOrderIsFlipped) :
            KTPhysicalArray< 2, std::complex<double> >(nBins0, rangeMin0, rangeMax0, nBins1, rangeMin1, rangeMax1),
            KTChirpSpace(),
            fIsArrayOrderFlipped(arrayOrderIsFlipped),
            fIsSizeEven(nBins0 == 0),
            fLeftOfCenterOffset((nBins0+1)/2),
            fCenterBin(nBins0/2),
            fNTimeBins(0),
            fPointCache()
    {
        if (arrayOrderIsFlipped)
        {
            fConstBinAccess = &KTChirpSpaceFFT::ReorderedBinAccess;
            fBinAccess = &KTChirpSpaceFFT::ReorderedBinAccess;
        }
        else
        {
            fConstBinAccess = &KTChirpSpaceFFT::AsIsBinAccess;
            fBinAccess = &KTChirpSpaceFFT::AsIsBinAccess;
        }
        //KTINFO(fslog, "number of bins: " << nBins << "   is size even? " << fIsSizeEven);
        //KTINFO(fslog, "neg freq offset: " << fLeftOfCenterOffset);
    }

    // I have doubts about this function
    KTChirpSpaceFFT::KTChirpSpaceFFT(std::initializer_list<double> value, size_t nBins0, double rangeMin0, double rangeMax0, size_t nBins1, double rangeMin1, double rangeMax1, bool arrayOrderIsFlipped) :
            KTChirpSpaceFFT(nBins0, rangeMin0, rangeMax0, nBins1, rangeMin1, rangeMax1, arrayOrderIsFlipped)
    {
        for (unsigned x_index = 0; x_index < nBins0; ++x_index)
        {
	    for (unsigned y_index = 0; y_index < nBins0; ++y_index)
	    {
            	std::copy(value.begin(), value.end(), &fData(x_index,y_index));
	    }
        }
        //std::copy(value.begin(), value.end(), this->begin());
    }

/*
    const KTAxisProperties< 2 >& KTChirpSpaceFFT::GetAxis() const
    {
        return *this;
    }

    KTAxisProperties< 2 >& KTChirpSpaceFFT::GetAxis()
    {
        return *this;
    }
*/
    KTChirpSpaceFFT& KTChirpSpaceFFT::CConjugate()
    {

        fData = conj(fData);
        return *this;
    }

    KTChirpSpaceFFT& KTChirpSpaceFFT::AnalyticAssociate()
    {
        // This is only valid if the original signal is Real only (not complex)
        // Note: the data storage array is accessed directly, so the FFTW data storage format is used.
        // Nyquist bin(s) and negative frequency bins are set to 0 (from size/2 to the end of the array)
        // DC bin stays as is (array position 0).
        // Positive frequency bins are multiplied by 2 (from array position 1 to size/2).
        unsigned nBins = size(0);
        unsigned nyquistPos = nBins / 2; // either the sole nyquist bin (if even # of bins) or the first of the two (if odd # of bins; bins are sequential in the array).
        
        //fData.segment(1,nyquistPos-1) *= 2.0 ;
	for(int i=0; i<nyquistPos; i++)
	{
		fData(i,0) *= 2.0 ;
	}
        //fData.tail(nBins-nyquistPos) = std::complex<double> {0.0};
	for(int i=0; i<nBins-nyquistPos; i++)
	{
		fData(nBins-i-1,0) = std::complex<double> {0.0};
	}

        return *this;
    }

    KTChirpSpaceFFT& KTChirpSpaceFFT::Scale(double scale)
    {
        
        fData *= scale;
        return *this;
    }

/*
    // todo -> Replace KTFrequencySpecrumPolar 
    KTFrequencySpecrumPolar* KTChirpSpaceFFT::CreateFrequencySpectrumPolar() const
    {
        unsigned nBins = size(0);
        KTFrequencySpecrumPolar* newFS = new KTFrequencySpecrumPolar(nBins, GetRangeMin(), GetRangeMax());
        newFS->SetNTimeBins(fNTimeBins);
#pragma omp parallel for
        for (unsigned iBin=0; iBin<nBins; ++iBin)
        {
           (*newFS)(iBin).set_rect((*this)(iBin).real(), (*this)(iBin).imag());
        }
        return newFS;
    }
*/
    KTPowerSpectrum* KTChirpSpaceFFT::CreatePowerSpectrum() const
    {
        // This function creates a power spectrum that runs from the smallest to the largest absolute frequency.
        // It can handle frequency ranges that do or don't cross DC, and that are symmetric or asymmetric.
        // If the frequency range does not cross DC, the power spectrum range will not either.
        // If the frequency range crosses DC, the power spectrum range will run from DC to the maximum absolute frequency
        // In this case negative-frequency bins are added to positive-frequency bins.

        double maxFreq = std::max(fabs(GetRangeMin(0)), fabs(GetRangeMax(0)));
        double minFreq = -0.5 * GetBinWidth(0);
        unsigned nBins = (maxFreq - minFreq) / GetBinWidth(0);
        if (GetRangeMax(0) < 0. || GetRangeMin(0) > 0.)
        {
            minFreq = std::min(fabs(GetRangeMin(0)), fabs(GetRangeMax(0)));
            nBins = size(0);
        }

        KTPowerSpectrum* newPS = new KTPowerSpectrum(nBins, minFreq, maxFreq);
        
        // to replace after Eigen replaces all KTPhysicalArrays
        for (unsigned iBin = 0; iBin < nBins; ++iBin) (*newPS)(iBin) = 0.;
       // newPS->GetData().setZero(); //probably unnecessary, Eigen array should be zero-initialized

        int dcBin = FindBin(0,0.);
        // default case: dcBin >= 0 && dcBin < size(0)
        int firstPosFreqBin = dcBin;
        int lastPosFreqBin = size(0);
        int firstNegFreqBin = 0;
        int lastNegFreqBin = dcBin;
        if (dcBin >= (int)size(0))
        {
            firstPosFreqBin = size(0); // lastPosFreqBin = size(0);
            lastNegFreqBin = size(0); // firstNEgFreqBin = 0
        }
        else if (dcBin < 0)
        {
            firstPosFreqBin = 0; // lastPosFreqBin = size(0);
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
            valueReal = (*this)(iBin,0).real();
            valueImag = (*this)(iBin,0).imag();
            (*newPS)(iBin) = (valueReal * valueReal + valueImag * valueImag) * scaling;
        }
#pragma omp parallel for private(valueReal, valueImag)
        for (unsigned iBin = firstNegFreqBin; iBin < lastNegFreqBin; ++iBin)
        {
            valueReal = (*this)(iBin,0).real();
            valueImag = (*this)(iBin,0).imag();
            (*newPS)(iBin) = (valueReal * valueReal + valueImag * valueImag) * scaling;
        }
                    
        return newPS;
    }

    void KTChirpSpaceFFT::Print(unsigned startPrint, unsigned nToPrint) const
    {
        stringstream printStream;
        for (unsigned iBin = startPrint; iBin < startPrint + nToPrint; ++iBin)
        {
            // order matters, so use (*this)() to access values
            printStream << "Bin " << iBin << ";   x = " << GetBinCenter(iBin,0) <<
                    ";   y = " << (*this)(iBin,0) << "\n";
        }
        KTDEBUG(fslog, "\n" << printStream.str());
        return;
    }

} /* namespace Katydid */
