/**
 @file KTFrequencySpectrumFFTW.hh
 @brief Contains KTFrequencySpectrum
 @details 
 @author: N. S. Oblath
 @date: Aug 28, 2012
 */

#ifndef KTFREQUENCYSPECTRUMFFTW_HH_
#define KTFREQUENCYSPECTRUMFFTW_HH_

#include "KTFrequencySpectrum.hh"
#include "KTPhysicalArrayFFTW.hh"

#include <cmath>
#include <string>

namespace Katydid
{
    class KTFrequencySpectrumPolar;

    class KTFrequencySpectrumFFTW : public KTPhysicalArray< 1, fftw_complex >, public KTFrequencySpectrum
    {
        public:
            KTFrequencySpectrumFFTW();
            KTFrequencySpectrumFFTW(size_t nBins, double rangeMin=0., double rangeMax=1., bool arrayOrderIsFlipped=false);
            KTFrequencySpectrumFFTW(const KTFrequencySpectrumFFTW& orig);
            virtual ~KTFrequencySpectrumFFTW();

        public:
            bool GetIsArrayOrderFlipped() const;
            bool GetIsSizeEven() const;
            size_t GetLeftOfCenterOffset() const;
            size_t GetCenterBin() const;

        protected:
            bool fIsArrayOrderFlipped; /// Flag to indicate that the bins to the left of center are actually in the right half of the array in memory
            bool fIsSizeEven; /// Flag to indicate if the size of the array is even
            size_t fLeftOfCenterOffset; /// The number of bins by which the negative-frequency Nyquist bin is offset
            size_t fCenterBin; /// The bin number of the DC bin


        public:
            const KTAxisProperties< 1 >& GetAxis() const;
            KTAxisProperties< 1 >& GetAxis();

            // replace some of the KTPhysicalArray interface

            const fftw_complex& operator()(unsigned i) const;
            fftw_complex& operator()(unsigned i);

            virtual double GetReal(unsigned bin) const;
            virtual double GetImag(unsigned bin) const;

            virtual void SetRect(unsigned bin, double real, double imag);

            virtual double GetAbs(unsigned bin) const;
            virtual double GetArg(unsigned bin) const;

            virtual void SetPolar(unsigned bin, double abs, double arg);

            virtual unsigned GetNFrequencyBins() const;
            virtual double GetFrequencyBinWidth() const;

            virtual unsigned GetNTimeBins() const;
            virtual void SetNTimeBins(unsigned bins);

        private:
            typedef const fftw_complex& (KTFrequencySpectrumFFTW::*ConstOrderedBinAccessFunc)(unsigned) const;
            typedef fftw_complex& (KTFrequencySpectrumFFTW::*OrderedBinAccessFunc)(unsigned);

            ConstOrderedBinAccessFunc fConstBinAccess;
            OrderedBinAccessFunc fBinAccess;

            const fftw_complex& ReorderedBinAccess(unsigned i) const;
            fftw_complex& ReorderedBinAccess(unsigned i);

            const fftw_complex& AsIsBinAccess(unsigned i) const;
            fftw_complex& AsIsBinAccess(unsigned i);

        public:
            // normal KTFrequencySpectrumPolar functions

            virtual KTFrequencySpectrumFFTW& operator=(const KTFrequencySpectrumFFTW& rhs);

            /// In-place calculation of the complex conjugate
            virtual KTFrequencySpectrumFFTW& CConjugate();
            /// In-place calculation of the analytic associate
            virtual KTFrequencySpectrumFFTW& AnalyticAssociate();

            virtual KTFrequencySpectrumFFTW& Scale(double scale);

            virtual KTFrequencySpectrumPolar* CreateFrequencySpectrumPolar() const;
            virtual KTPowerSpectrum* CreatePowerSpectrum() const;

            void Print(unsigned startPrint, unsigned nToPrint) const;

        private:
            unsigned fNTimeBins;

        protected:
            mutable const fftw_complex* fPointCache;
    };

    inline bool KTFrequencySpectrumFFTW::GetIsArrayOrderFlipped() const
    {
        return fIsArrayOrderFlipped;
    }

    inline bool KTFrequencySpectrumFFTW::GetIsSizeEven() const
    {
        return fIsSizeEven;
    }

    inline size_t KTFrequencySpectrumFFTW::GetLeftOfCenterOffset() const
    {
        return fLeftOfCenterOffset;
    }

    inline size_t KTFrequencySpectrumFFTW::GetCenterBin() const
    {
        return fCenterBin;
    }

    inline const fftw_complex& KTFrequencySpectrumFFTW::operator()(unsigned i) const
    {
        return (this->*fConstBinAccess)(i);
    }

    inline fftw_complex& KTFrequencySpectrumFFTW::operator()(unsigned i)
    {
        return (this->*fBinAccess)(i);
    }

    inline const fftw_complex& KTFrequencySpectrumFFTW::ReorderedBinAccess(unsigned i) const
    {
        return (i >= fCenterBin) ? fData[i - fCenterBin] : fData[i + fLeftOfCenterOffset];
    }

    inline fftw_complex& KTFrequencySpectrumFFTW::ReorderedBinAccess(unsigned i)
    {
        return (i >= fCenterBin) ? fData[i - fCenterBin] : fData[i + fLeftOfCenterOffset];
    }

    inline const fftw_complex& KTFrequencySpectrumFFTW::AsIsBinAccess(unsigned i) const
    {
        return fData[i];
    }

    inline fftw_complex& KTFrequencySpectrumFFTW::AsIsBinAccess(unsigned i)
    {
        return fData[i];
    }

    inline double KTFrequencySpectrumFFTW::GetReal(unsigned bin) const
    {
        return (*this)(bin)[0];
    }

    inline double KTFrequencySpectrumFFTW::GetImag(unsigned bin) const
    {
        return (*this)(bin)[1];
    }

    inline void KTFrequencySpectrumFFTW::SetRect(unsigned bin, double real, double imag)
    {
        fPointCache = &(*this)(bin);
        (*const_cast< fftw_complex* >(fPointCache))[0] = real;
        (*const_cast< fftw_complex* >(fPointCache))[1] = imag;
        return;
    }

    inline double KTFrequencySpectrumFFTW::GetAbs(unsigned bin) const
    {
        fPointCache = &(*this)(bin);
        return sqrt((*fPointCache)[0]*(*fPointCache)[0] + (*fPointCache)[1]*(*fPointCache)[1]);
    }

    inline double KTFrequencySpectrumFFTW::GetArg(unsigned bin) const
    {
        fPointCache = &(*this)(bin);
        return atan2((*fPointCache)[1], (*fPointCache)[0]);
    }

    inline void KTFrequencySpectrumFFTW::SetPolar(unsigned bin, double abs, double arg)
    {
        fPointCache = &(*this)(bin);
        (*const_cast< fftw_complex* >(fPointCache))[0] = abs * cos(arg);
        (*const_cast< fftw_complex* >(fPointCache))[1] = abs * sin(arg);
        return;
    }

    inline unsigned KTFrequencySpectrumFFTW::GetNFrequencyBins() const
    {
        return size();
    }

    inline double KTFrequencySpectrumFFTW::GetFrequencyBinWidth() const
    {
        return GetBinWidth();
    }

    inline unsigned KTFrequencySpectrumFFTW::GetNTimeBins() const
    {
        return fNTimeBins;
    }

    inline void KTFrequencySpectrumFFTW::SetNTimeBins(unsigned bins)
    {
        fNTimeBins = bins;
        return;
    }

} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUMFFTW_HH_ */
