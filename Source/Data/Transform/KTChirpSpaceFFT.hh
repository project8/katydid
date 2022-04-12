/**
 @file KTChirpSpaceFFT.hh
 @brief Contains KTChirpSpace
 @details 
 @author: N. S. Oblath
 @date: Aug 28, 2012
 */

#ifndef KTCHIRPSPACEFFT_HH_
#define KTCHIRPSPACEFFT_HH_

#include "KTChirpSpace.hh"
#include "KTPhysicalArrayComplex.hh"

#include <cmath>
#include <string>

namespace Katydid
{
    
    //class KTFrequencySpectrumPolar;

    class KTChirpSpaceFFT : public KTPhysicalArray< 2, std::complex<double> >, public KTChirpSpace
    {
        public:
            KTChirpSpaceFFT();
            KTChirpSpaceFFT(size_t nBins0=1, double rangeMin0=0., double rangeMax0=1.,size_t nBins1=1, double rangeMin1=0., double rangeMax1=1., bool arrayOrderIsFlipped=false);
            KTChirpSpaceFFT(std::initializer_list<double> value, size_t nBins0=1, double rangeMin0=0., double rangeMax0=1.,size_t nBins1=1, double rangeMin1=0., double rangeMax1=1., bool arrayOrderIsFlipped=false);

            virtual ~KTChirpSpaceFFT() = default;

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
//            const KTAxisProperties< 2 >& GetAxis() const;
//            KTAxisProperties< 2 >& GetAxis();

            const std::string& GetOrdinateLabel() const;

            // replace some of the KTPhysicalArray interface

            const std::complex<double>& operator()(unsigned i, unsigned j) const;
            std::complex<double>& operator()(unsigned i, unsigned j);

            virtual double GetReal(unsigned SlopeComponent, unsigned InterceptComponent) const;
            virtual double GetImag(unsigned SlopeComponent, unsigned InterceptComponent) const;

            virtual void SetRect(unsigned SlopeComponent, unsigned InterceptComponent, double real, double imag);

            virtual double GetAbs(unsigned SlopeComponent, unsigned InterceptComponent) const;
            virtual double GetArg(unsigned SlopeComponent, unsigned InterceptComponent) const;
            virtual double GetNorm(unsigned SlopeComponent, unsigned InterceptComponent) const;

            //virtual void SetPolar(unsigned bin, double abs, double arg);

            virtual unsigned GetNSlopeBins() const;
            virtual double GetSlopeBinWidth() const;

            virtual unsigned GetNInterceptBins() const;
            virtual double GetInterceptBinWidth() const;

            virtual unsigned GetNTimeBins() const;
            virtual void SetNTimeBins(unsigned bins);

        private:
            typedef const std::complex<double>& (KTChirpSpaceFFT::*ConstOrderedBinAccessFunc)(unsigned, unsigned) const;
            typedef std::complex<double>& (KTChirpSpaceFFT::*OrderedBinAccessFunc)(unsigned, unsigned);

            ConstOrderedBinAccessFunc fConstBinAccess;
            OrderedBinAccessFunc fBinAccess;

            const std::complex<double>& ReorderedBinAccess(unsigned i, unsigned j) const;
            std::complex<double>& ReorderedBinAccess(unsigned i, unsigned j);

            const std::complex<double>& AsIsBinAccess(unsigned i, unsigned j) const;
            std::complex<double>& AsIsBinAccess(unsigned i, unsigned j);

        public:
            // normal KTFrequencySpectrumPolar functions

            /// In-place calculation of the complex conjugate
            virtual KTChirpSpaceFFT& CConjugate();
            /// In-place calculation of the analytic associate
            virtual KTChirpSpaceFFT& AnalyticAssociate();

            virtual KTChirpSpaceFFT& Scale(double scale);

            //virtual KTFrequencySpectrumPolar* CreateFrequencySpectrumPolar() const;
            virtual KTPowerSpectrum* CreatePowerSpectrum() const;

            void Print(unsigned startPrint, unsigned nToPrint) const;

        private:
            unsigned fNTimeBins;

        protected:
            mutable const std::complex<double>* fPointCache;
    };

    inline bool KTChirpSpaceFFT::GetIsArrayOrderFlipped() const
    {
        return fIsArrayOrderFlipped;
    }

    inline bool KTChirpSpaceFFT::GetIsSizeEven() const
    {
        return fIsSizeEven;
    }

    inline size_t KTChirpSpaceFFT::GetLeftOfCenterOffset() const
    {
        return fLeftOfCenterOffset;
    }

    inline size_t KTChirpSpaceFFT::GetCenterBin() const
    {
        return fCenterBin;
    }

    inline const std::string& KTChirpSpaceFFT::GetOrdinateLabel() const
    {
        return GetDataLabel();
    }

    inline const std::complex<double>& KTChirpSpaceFFT::operator()(unsigned i, unsigned j) const
    {
        return (this->*fConstBinAccess)(i,j);
    }

    inline std::complex<double>& KTChirpSpaceFFT::operator()(unsigned i, unsigned j)
    {
        return (this->*fBinAccess)(i, j);
    }

    inline const std::complex<double>& KTChirpSpaceFFT::ReorderedBinAccess(unsigned i, unsigned j) const
    {
        return (i >= fCenterBin) ? fData(i - fCenterBin, j) : fData(i + fLeftOfCenterOffset, j);
    }

    inline std::complex<double>& KTChirpSpaceFFT::ReorderedBinAccess(unsigned i, unsigned j)
    {
        return (i >= fCenterBin) ? fData(i - fCenterBin, j) : fData(i + fLeftOfCenterOffset, j);
    }

    inline const std::complex<double>& KTChirpSpaceFFT::AsIsBinAccess(unsigned i, unsigned j) const
    {
        return fData(i, j);
    }

    inline std::complex<double>& KTChirpSpaceFFT::AsIsBinAccess(unsigned i, unsigned j)
    {
        return fData(i,j);
    }

    inline double KTChirpSpaceFFT::GetReal(unsigned SlopeComponent, unsigned InterceptComponent) const
    {
        return (*this)(SlopeComponent, InterceptComponent).real();
    }

    inline double KTChirpSpaceFFT::GetImag(unsigned SlopeComponent, unsigned InterceptComponent) const
    {
        return (*this)(SlopeComponent, InterceptComponent).imag();
    }

    inline void KTChirpSpaceFFT::SetRect(unsigned SlopeComponent, unsigned InterceptComponent, double real, double imag)
    {
        (*this)(SlopeComponent, InterceptComponent) = std::complex<double>(real, imag);
        return;
    }

    inline double KTChirpSpaceFFT::GetAbs(unsigned SlopeComponent, unsigned InterceptComponent) const
    {
        return std::abs((*this)(SlopeComponent, InterceptComponent));
    }
    
    inline double KTChirpSpaceFFT::GetNorm(unsigned SlopeComponent, unsigned InterceptComponent) const
    {
        return std::norm((*this)(SlopeComponent, InterceptComponent));
    }

    inline double KTChirpSpaceFFT::GetArg(unsigned SlopeComponent, unsigned InterceptComponent) const
    {
        return std::arg((*this)(SlopeComponent, InterceptComponent));
    }

/*    inline void KTChirpSpaceFFT::SetPolar(unsigned bin, double abs, double arg)
    {
        (*this)(bin) = std::polar(abs, arg);
        return;
    }
*/
    inline unsigned KTChirpSpaceFFT::GetNSlopeBins() const
    {
        return size(0);
    }

    inline double KTChirpSpaceFFT::GetSlopeBinWidth() const
    {
        return GetBinWidth(0);
    }

    inline unsigned KTChirpSpaceFFT::GetNInterceptBins() const
    {
        return size(1);
    }

    inline double KTChirpSpaceFFT::GetInterceptBinWidth() const
    {
        return GetBinWidth(1);
    }

    inline unsigned KTChirpSpaceFFT::GetNTimeBins() const
    {
        return fNTimeBins;
    }

    inline void KTChirpSpaceFFT::SetNTimeBins(unsigned bins)
    {
        fNTimeBins = bins;
        return;
    }

} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUMFFTW_HH_ */
