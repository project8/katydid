/*
 * KTFrequencySpectrumFFTW.hh
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMFFTW_HH_
#define KTFREQUENCYSPECTRUMFFTW_HH_

#include "KTFrequencySpectrum.hh"
#include "KTPhysicalArrayFFTW.hh"

#include "Rtypes.h"

#include <string>

#ifdef ROOT_FOUND
class TH1D;
#endif

namespace Katydid
{
    class KTFrequencySpectrumPolar;
    class KTPowerSpectrum;

    class KTFrequencySpectrumFFTW : public KTPhysicalArray< 1, fftw_complex >, public KTFrequencySpectrum
    {
        public:
            KTFrequencySpectrumFFTW();
            KTFrequencySpectrumFFTW(size_t nBins, Double_t rangeMin=0., Double_t rangeMax=1.);
            KTFrequencySpectrumFFTW(const KTFrequencySpectrumFFTW& orig);
            virtual ~KTFrequencySpectrumFFTW();

        public:
            Bool_t GetIsSizeEven() const;
            size_t GetNegFreqOffset() const;
            size_t GetDCBin() const;

        protected:
            Bool_t fIsSizeEven; /// Flag to indicate if the size of the array is even
            size_t fNegFreqOffset; /// The number of bins by which the negative-frequency Nyquist bin is offset
            size_t fDCBin; /// The bin number of the DC bin


        public:
            // replace some of the KTPhysicalArray interface

            const fftw_complex& operator()(unsigned i) const;
            fftw_complex& operator()(unsigned i);

            /// Returns the size of the positive-frequency part of the array
            //size_t size() const;
            /// Returns the isze of the positive-frequency part of the array
            //size_t GetNBins() const;

            /// Returns the actual size of the storage array
            //size_t size_total() const;
            /// Returns the actual size of the storage array
            //size_t GetNBinsTotal() const;

        public:
            // normal KTFrequencySpectrumPolar functions

            virtual KTFrequencySpectrumFFTW& operator=(const KTFrequencySpectrumFFTW& rhs);

            /// In-place calculation of the complex conjugate
            virtual KTFrequencySpectrumFFTW& CConjugate();
            /// In-place calculation of the analytic associate
            virtual KTFrequencySpectrumFFTW& AnalyticAssociate();

            virtual KTFrequencySpectrumPolar* CreateFrequencySpectrum() const;
            virtual KTPowerSpectrum* CreatePowerSpectrum() const;

            void Print(unsigned startPrint, unsigned nToPrint) const;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH1D* CreatePhaseHistogram(const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH1D* CreatePowerHistogram(const std::string& name = "hFrequencySpectrumPower") const;

            virtual TH1D* CreatePowerDistributionHistogram(const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

    inline Bool_t KTFrequencySpectrumFFTW::GetIsSizeEven() const
    {
        return fIsSizeEven;
    }

    inline size_t KTFrequencySpectrumFFTW::GetNegFreqOffset() const
    {
        return fNegFreqOffset;
    }

    inline size_t KTFrequencySpectrumFFTW::GetDCBin() const
    {
        return fDCBin;
    }

    inline const fftw_complex& KTFrequencySpectrumFFTW::operator()(unsigned i) const
    {
        return (i >= fDCBin) ? fData[i - fDCBin] : fData[i + fNegFreqOffset];
        //return fData[i];
    }

    inline fftw_complex& KTFrequencySpectrumFFTW::operator()(unsigned i)
    {
        return (i >= fDCBin) ? fData[i - fDCBin] : fData[i + fNegFreqOffset];
        //return fData[i];
    }



} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUMFFTW_HH_ */
