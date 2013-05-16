/*
 * KTTimeFrequencyPolar.hh
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#ifndef KTTIMEFREQUENCYPOLAR_HH_
#define KTTIMEFREQUENCYPOLAR_HH_

#include "complexpolar.hh"
#include "KTTimeFrequency.hh"
#include "KTPhysicalArray.hh"

#include <cmath>
//#include <string>

#ifdef ROOT_FOUND
class TH2D;
#endif

namespace Katydid
{
    //class KTPowerSpectrum;

    class KTTimeFrequencyPolar : public KTPhysicalArray< 2, complexpolar< Double_t > >, public KTTimeFrequency
    {
        public:
            KTTimeFrequencyPolar();
            KTTimeFrequencyPolar(size_t nTimeBins, Double_t timeRangeMin, Double_t timeRangeMax, size_t nFreqBins, Double_t freqRangeMin, Double_t freqRangeMax);
            KTTimeFrequencyPolar(const KTTimeFrequencyPolar& orig);
            virtual ~KTTimeFrequencyPolar();

            virtual KTTimeFrequencyPolar& operator=(const KTTimeFrequencyPolar& rhs);

            virtual Double_t GetReal(UInt_t timebin, UInt_t freqbin) const;
            virtual Double_t GetImag(UInt_t timebin, UInt_t freqbin) const;

            virtual void SetRect(UInt_t timebin, UInt_t freqbin, Double_t real, Double_t imag);

            virtual Double_t GetAbs(UInt_t timebin, UInt_t freqbin) const;
            virtual Double_t GetArg(UInt_t timebin, UInt_t freqbin) const;

            virtual void SetPolar(UInt_t timebin, UInt_t freqbin, Double_t abs, Double_t arg);

            virtual UInt_t GetNTimeBins() const;
            virtual UInt_t GetNFrequencyBins() const;

            virtual Double_t GetTimeBinWidth() const;
            virtual Double_t GetFrequencyBinWidth() const;

            virtual KTTimeFrequencyPolar& CConjugate();

            //virtual KTPowerSpectrum* CreatePowerSpectrum() const;

            //void Print(unsigned startPrint, unsigned nToPrint) const;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateMagnitudeHistogram(const std::string& name = "hTimeFrequencyMag") const;
            virtual TH2D* CreatePhaseHistogram(const std::string& name = "hTimeFrequencyPhase") const;

            virtual TH2D* CreatePowerHistogram(const std::string& name = "hTimeFrequencyPower") const;

            //virtual TH1D* CreatePowerDistributionHistogram(const std::string& name = "hTimeFrequencyPowerDist") const;
#endif

    };

    inline Double_t KTTimeFrequencyPolar::GetReal(UInt_t timebin, UInt_t freqbin) const
    {
        return (*this)(timebin, freqbin).abs() * std::cos((*this)(timebin, freqbin).arg());
    }

    inline Double_t KTTimeFrequencyPolar::GetImag(UInt_t timebin, UInt_t freqbin) const
    {
        return (*this)(timebin, freqbin).abs() * std::sin((*this)(timebin, freqbin).arg());
    }

    inline void KTTimeFrequencyPolar::SetRect(UInt_t timebin, UInt_t freqbin, Double_t real, Double_t imag)
    {
        (*this)(timebin, freqbin).set_rect(real, imag);
        return;
    }

    inline Double_t KTTimeFrequencyPolar::GetAbs(UInt_t timebin, UInt_t freqbin) const
    {
        return (*this)(timebin, freqbin).abs();
    }

    inline Double_t KTTimeFrequencyPolar::GetArg(UInt_t timebin, UInt_t freqbin) const
    {
        return (*this)(timebin, freqbin).arg();
    }

    inline void KTTimeFrequencyPolar::SetPolar(UInt_t timebin, UInt_t freqbin, Double_t abs, Double_t arg)
    {
        (*this)(timebin, freqbin).set_polar(abs, arg);
        return;
    }

    inline UInt_t KTTimeFrequencyPolar::GetNTimeBins() const
    {
        return size(1);
    }

    inline UInt_t KTTimeFrequencyPolar::GetNFrequencyBins() const
    {
        return size(2);
    }

    inline Double_t KTTimeFrequencyPolar::GetTimeBinWidth() const
    {
        return GetBinWidth(1);
    }

    inline Double_t KTTimeFrequencyPolar::GetFrequencyBinWidth() const
    {
        return GetBinWidth(2);
    }



} /* namespace Katydid */
#endif /* KTTIMEFREQUENCYPOLAR_HH_ */
