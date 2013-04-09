/*
 * KTFrequencySpectrumPolar.hh
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMPOLAR_HH_
#define KTFREQUENCYSPECTRUMPOLAR_HH_

#include "complexpolar.hh"
#include "KTFrequencySpectrum.hh"
#include "KTPhysicalArray.hh"

#include "Rtypes.h"

#include <cmath>
#include <string>

#ifdef ROOT_FOUND
class TH1D;
#endif

namespace Katydid
{
    class KTPowerSpectrum;

    class KTFrequencySpectrumPolar : public KTPhysicalArray< 1, complexpolar< Double_t > >, public KTFrequencySpectrum
    {
        public:
            KTFrequencySpectrumPolar();
            KTFrequencySpectrumPolar(size_t nBins, Double_t rangeMin=0., Double_t rangeMax=1.);
            KTFrequencySpectrumPolar(const KTFrequencySpectrumPolar& orig);
            virtual ~KTFrequencySpectrumPolar();

            virtual KTFrequencySpectrumPolar& operator=(const KTFrequencySpectrumPolar& rhs);

            virtual Double_t GetReal(UInt_t bin) const;
            virtual Double_t GetImag(UInt_t bin) const;

            virtual void SetRect(UInt_t bin, Double_t real, Double_t imag);

            virtual Double_t GetAbs(UInt_t bin) const;
            virtual Double_t GetArg(UInt_t bin) const;

            virtual void SetPolar(UInt_t bin, Double_t abs, Double_t arg);

            virtual UInt_t GetNFrequencyBins() const;
            virtual Double_t GetFrequencyBinWidth() const;

            virtual KTFrequencySpectrumPolar& CConjugate();

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

    inline Double_t KTFrequencySpectrumPolar::GetReal(UInt_t bin) const
    {
        return (*this)(bin).abs() * std::cos((*this)(bin).arg());
    }

    inline Double_t KTFrequencySpectrumPolar::GetImag(UInt_t bin) const
    {
        return (*this)(bin).abs() * std::sin((*this)(bin).arg());
    }

    inline void KTFrequencySpectrumPolar::SetRect(UInt_t bin, Double_t real, Double_t imag)
    {
        (*this)(bin).set_rect(real, imag);
        return;
    }

    inline Double_t KTFrequencySpectrumPolar::GetAbs(UInt_t bin) const
    {
        return (*this)(bin).abs();
    }

    inline Double_t KTFrequencySpectrumPolar::GetArg(UInt_t bin) const
    {
        return (*this)(bin).arg();
    }

    inline void KTFrequencySpectrumPolar::SetPolar(UInt_t bin, Double_t abs, Double_t arg)
    {
        (*this)(bin).set_polar(abs, arg);
        return;
    }

    inline UInt_t KTFrequencySpectrumPolar::GetNFrequencyBins() const
    {
        return size();
    }

    inline Double_t KTFrequencySpectrumPolar::GetFrequencyBinWidth() const
    {
        return GetBinWidth();
    }



} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUMPOLAR_HH_ */
