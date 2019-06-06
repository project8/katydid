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
            KTTimeFrequencyPolar(complexpolar< double > value, size_t nTimeBins, double timeRangeMin, double timeRangeMax, size_t nFreqBins, double freqRangeMin, double freqRangeMax);
            KTTimeFrequencyPolar(const KTTimeFrequencyPolar& orig);
            virtual ~KTTimeFrequencyPolar();

            virtual KTTimeFrequencyPolar& operator=(const KTTimeFrequencyPolar& rhs);

            virtual double GetReal(unsigned timebin, unsigned freqbin) const;
            virtual double GetImag(unsigned timebin, unsigned freqbin) const;

            virtual void SetRect(unsigned timebin, unsigned freqbin, double real, double imag);

            virtual double GetAbs(unsigned timebin, unsigned freqbin) const;
            virtual double GetArg(unsigned timebin, unsigned freqbin) const;

            virtual void SetPolar(unsigned timebin, unsigned freqbin, double abs, double arg);

            virtual unsigned GetNTimeBins() const;
            virtual unsigned GetNFrequencyBins() const;

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

    inline double KTTimeFrequencyPolar::GetReal(unsigned timebin, unsigned freqbin) const
    {
        return (*this)(timebin, freqbin).abs() * std::cos((*this)(timebin, freqbin).arg());
    }

    inline double KTTimeFrequencyPolar::GetImag(unsigned timebin, unsigned freqbin) const
    {
        return (*this)(timebin, freqbin).abs() * std::sin((*this)(timebin, freqbin).arg());
    }

    inline void KTTimeFrequencyPolar::SetRect(unsigned timebin, unsigned freqbin, double real, double imag)
    {
        (*this)(timebin, freqbin).set_rect(real, imag);
        return;
    }

    inline double KTTimeFrequencyPolar::GetAbs(unsigned timebin, unsigned freqbin) const
    {
        return (*this)(timebin, freqbin).abs();
    }

    inline double KTTimeFrequencyPolar::GetArg(unsigned timebin, unsigned freqbin) const
    {
        return (*this)(timebin, freqbin).arg();
    }

    inline void KTTimeFrequencyPolar::SetPolar(unsigned timebin, unsigned freqbin, double abs, double arg)
    {
        (*this)(timebin, freqbin).set_polar(abs, arg);
        return;
    }

    inline unsigned KTTimeFrequencyPolar::GetNTimeBins() const
    {
        return size(1);
    }

    inline unsigned KTTimeFrequencyPolar::GetNFrequencyBins() const
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
