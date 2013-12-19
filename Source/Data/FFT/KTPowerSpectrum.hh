/*
 * KTPowerSpectrum.hh
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#ifndef KTPOWERSPECTRUM_HH_
#define KTPOWERSPECTRUM_HH_

//#include "KTComplexVector.hh"

#include "KTPhysicalArray.hh"

#include <string>

class TH1D;

namespace Katydid
{

    class KTPowerSpectrum : public KTPhysicalArray< 1, double >
    {
        public:
            KTPowerSpectrum();
            KTPowerSpectrum(size_t nBins, double rangeMin=0., double rangeMax=1.);
            KTPowerSpectrum(const KTPowerSpectrum& orig);
            virtual ~KTPowerSpectrum();

        public:
            /// Returns the resistance used to calculate the power from the voltage in a frequency spectrum
            static double GetResistance();

        protected:
            const static double fResistance; // ohms

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreatePowerHistogram(const std::string& name = "hPowerSpectrum") const;

            virtual TH1D* CreatePowerDistributionHistogram(const std::string& name = "hPowerSpectrumPower") const;
#endif
    };


    /*
    class KTPowerSpectrum : public KTComplexVector
    {
        public:
            KTPowerSpectrum();
            KTPowerSpectrum(unsigned nBins, double binWidth=1.);
            KTPowerSpectrum(const KTPowerSpectrum& original);
            virtual ~KTPowerSpectrum();

            /// Input should be the direct FFT, not a power spectrum
            virtual void TakeFrequencySpectrum(const KTComplexVector& freqSpect);
            virtual void TakeFrequencySpectrum(unsigned int nBins, const double* real, const double* imag);

            double GetPowerAtFrequency(double freq);
            double GetPhaseAtFrequency(double freq);
            int GetBin(double freq);

            virtual TH1D* CreateMagnitudeHistogram() const;
            virtual TH1D* CreateMagnitudeHistogram(const std::string& name) const;
            virtual TH1D* CreatePhaseHistogram() const;
            virtual TH1D* CreatePhaseHistogram(const std::string& name) const;

            virtual KTPhysicalArray< 1, double >* CreateMagnitudePhysArr() const;
            virtual KTPhysicalArray< 1, double >* CreatePhasePhysArr() const;

            virtual TH1D* CreatePowerDistributionHistogram() const;
            virtual TH1D* CreatePowerDistributionHistogram(const std::string& name) const;

            double GetBinWidth() const;

            void SetBinWidth(double bw);

        protected:
            double fBinWidth;

            ClassDef(KTPowerSpectrum, 2);
    };

    inline double KTPowerSpectrum::GetPowerAtFrequency(double freq)
    {
        return fMagnitude[GetBin(freq)];
    }

    inline double KTPowerSpectrum::GetPhaseAtFrequency(double freq)
    {
        return fPhase[GetBin(freq)];
    }

    inline int KTPowerSpectrum::GetBin(double freq)
    {
        return int(freq / fBinWidth);
    }

    inline double KTPowerSpectrum::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTPowerSpectrum::SetBinWidth(double bw)
    {
        fBinWidth = bw;
        return;
    }
    */

} /* namespace Katydid */
#endif /* KTPOWERSPECTRUM_HH_ */
