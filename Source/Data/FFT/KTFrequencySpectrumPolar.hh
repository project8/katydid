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

#include <cmath>
#include <string>

#ifdef ROOT_FOUND
class TH1D;
#endif

namespace Katydid
{
    class KTPowerSpectrum;

    class KTFrequencySpectrumPolar : public KTPhysicalArray< 1, complexpolar< double > >, public KTFrequencySpectrum
    {
        public:
            KTFrequencySpectrumPolar();
            KTFrequencySpectrumPolar(size_t nBins, double rangeMin=0., double rangeMax=1.);
            KTFrequencySpectrumPolar(const KTFrequencySpectrumPolar& orig);
            virtual ~KTFrequencySpectrumPolar();

            virtual KTFrequencySpectrumPolar& operator=(const KTFrequencySpectrumPolar& rhs);

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

            virtual KTFrequencySpectrumPolar& CConjugate();

            virtual KTFrequencySpectrumPolar& Scale(double scale);

            virtual KTPowerSpectrum* CreatePowerSpectrum() const;

            void Print(unsigned startPrint, unsigned nToPrint) const;

        private:
            unsigned fNTimeBins;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH1D* CreatePhaseHistogram(const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH1D* CreatePowerHistogram(const std::string& name = "hFrequencySpectrumPower") const;

            virtual TH1D* CreateMagnitudeDistributionHistogram(const std::string& name = "hFrequencySpectrumMagDist") const;
            virtual TH1D* CreatePowerDistributionHistogram(const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

    inline double KTFrequencySpectrumPolar::GetReal(unsigned bin) const
    {
        return (*this)(bin).abs() * std::cos((*this)(bin).arg());
    }

    inline double KTFrequencySpectrumPolar::GetImag(unsigned bin) const
    {
        return (*this)(bin).abs() * std::sin((*this)(bin).arg());
    }

    inline void KTFrequencySpectrumPolar::SetRect(unsigned bin, double real, double imag)
    {
        (*this)(bin).set_rect(real, imag);
        return;
    }

    inline double KTFrequencySpectrumPolar::GetAbs(unsigned bin) const
    {
        return (*this)(bin).abs();
    }

    inline double KTFrequencySpectrumPolar::GetArg(unsigned bin) const
    {
        return (*this)(bin).arg();
    }

    inline void KTFrequencySpectrumPolar::SetPolar(unsigned bin, double abs, double arg)
    {
        (*this)(bin).set_polar(abs, arg);
        return;
    }

    inline unsigned KTFrequencySpectrumPolar::GetNFrequencyBins() const
    {
        return size();
    }

    inline double KTFrequencySpectrumPolar::GetFrequencyBinWidth() const
    {
        return GetBinWidth();
    }

    inline unsigned KTFrequencySpectrumPolar::GetNTimeBins() const
    {
        return fNTimeBins;
    }

    inline void KTFrequencySpectrumPolar::SetNTimeBins(unsigned bins)
    {
        fNTimeBins = bins;
        return;
    }

} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUMPOLAR_HH_ */
