/*
 * KTPowerSpectrum.hh
 *
 *  Created on: Sep 12, 2011
 *      Author: nsoblath
 */

#ifndef KTPOWERSPECTRUM_HH_
#define KTPOWERSPECTRUM_HH_

//#include "KTComplexVector.hh"

#include "KTFrequencyDomainArray.hh"
#include "KTPhysicalArray.hh"

#include <string>

namespace Katydid
{

    class KTPowerSpectrum : public KTPhysicalArray< 1, double >, public KTFrequencyDomainArray
    {
        public:
            enum Mode
            {
                kPower,
                kPSD
            };

        public:
            KTPowerSpectrum(size_t nBins=1, double rangeMin=0., double rangeMax=1.);
            KTPowerSpectrum(const KTPowerSpectrum& orig);
            virtual ~KTPowerSpectrum();

            unsigned GetNFrequencyBins() const;
            double GetFrequencyBinWidth() const;

            const KTAxisProperties< 1 >& GetAxis() const;
            KTAxisProperties< 1 >& GetAxis();

            void ConvertToPowerSpectrum();
            void ConvertToPowerSpectralDensity();

            bool IsPowerSpectrum() const;
            bool IsPowerSpectralDensity() const;

        public:
            KTPowerSpectrum& operator=(const KTPowerSpectrum& rhs);

            KTPowerSpectrum& Scale(double scale);

            Mode GetMode() const;
            void SetMode(Mode mode);
            void OverrideMode(Mode mode);

            /// Returns the resistance used to calculate the power from the voltage in a frequency spectrum
            static double GetResistance();

        protected:
            Mode fMode;

            const static double fResistance; // ohms

    };

    inline unsigned KTPowerSpectrum::GetNFrequencyBins() const
    {
        return size();
    }

    inline double KTPowerSpectrum::GetFrequencyBinWidth() const
    {
        return GetBinWidth();
    }

    inline bool KTPowerSpectrum::IsPowerSpectrum() const
    {
        return fMode == kPower;
    }

    inline bool KTPowerSpectrum::IsPowerSpectralDensity() const
    {
        return fMode == kPSD;
    }

    inline void KTPowerSpectrum::SetMode(KTPowerSpectrum::Mode mode)
    {
        if (mode == kPSD) ConvertToPowerSpectralDensity();
        else ConvertToPowerSpectrum();
        return;
    }

    inline void KTPowerSpectrum::OverrideMode(KTPowerSpectrum::Mode mode)
    {
        fMode = mode;
        return;
    }

    inline KTPowerSpectrum::Mode KTPowerSpectrum::GetMode() const
    {
        return fMode;
    }

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
