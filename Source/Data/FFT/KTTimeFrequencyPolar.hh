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

    class KTTimeFrequencyPolar : public KTPhysicalArray< 2, complexpolar< double > >, public KTTimeFrequency
    {
        public:
            KTTimeFrequencyPolar();
            KTTimeFrequencyPolar(size_t nTimeBins, double timeRangeMin, double timeRangeMax, size_t nFreqBins, double freqRangeMin, double freqRangeMax);
            KTTimeFrequencyPolar(const KTTimeFrequencyPolar& orig);
            virtual ~KTTimeFrequencyPolar();

            virtual KTTimeFrequencyPolar& operator=(const KTTimeFrequencyPolar& rhs);

            virtual double GetReal(UInt_t timebin, UInt_t freqbin) const;
            virtual double GetImag(UInt_t timebin, UInt_t freqbin) const;

            virtual void SetRect(UInt_t timebin, UInt_t freqbin, double real, double imag);

            virtual double GetAbs(UInt_t timebin, UInt_t freqbin) const;
            virtual double GetArg(UInt_t timebin, UInt_t freqbin) const;

            virtual void SetPolar(UInt_t timebin, UInt_t freqbin, double abs, double arg);

            virtual UInt_t GetNTimeBins() const;
            virtual UInt_t GetNFrequencyBins() const;

            virtual double GetTimeBinWidth() const;
            virtual double GetFrequencyBinWidth() const;

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

    inline double KTTimeFrequencyPolar::GetReal(UInt_t timebin, UInt_t freqbin) const
    {
        return (*this)(timebin, freqbin).abs() * std::cos((*this)(timebin, freqbin).arg());
    }

    inline double KTTimeFrequencyPolar::GetImag(UInt_t timebin, UInt_t freqbin) const
    {
        return (*this)(timebin, freqbin).abs() * std::sin((*this)(timebin, freqbin).arg());
    }

    inline void KTTimeFrequencyPolar::SetRect(UInt_t timebin, UInt_t freqbin, double real, double imag)
    {
        (*this)(timebin, freqbin).set_rect(real, imag);
        return;
    }

    inline double KTTimeFrequencyPolar::GetAbs(UInt_t timebin, UInt_t freqbin) const
    {
        return (*this)(timebin, freqbin).abs();
    }

    inline double KTTimeFrequencyPolar::GetArg(UInt_t timebin, UInt_t freqbin) const
    {
        return (*this)(timebin, freqbin).arg();
    }

    inline void KTTimeFrequencyPolar::SetPolar(UInt_t timebin, UInt_t freqbin, double abs, double arg)
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

    inline double KTTimeFrequencyPolar::GetTimeBinWidth() const
    {
        return GetBinWidth(1);
    }

    inline double KTTimeFrequencyPolar::GetFrequencyBinWidth() const
    {
        return GetBinWidth(2);
    }



} /* namespace Katydid */
#endif /* KTTIMEFREQUENCYPOLAR_HH_ */
