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

#include "Rtypes.h"

#include <string>

class TH1D;

namespace Katydid
{

    class KTPowerSpectrum : public KTPhysicalArray< 1, Double_t >
    {
        public:
            KTPowerSpectrum();
            KTPowerSpectrum(size_t nBins, Double_t rangeMin=0., Double_t rangeMax=1.);
            KTPowerSpectrum(const KTPowerSpectrum& orig);
            virtual ~KTPowerSpectrum();

        public:
            /// Returns the resistance used to calculate the power from the voltage in a frequency spectrum
            static Double_t GetResistance();

        protected:
            const static Double_t fResistance; // ohms

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
            KTPowerSpectrum(UInt_t nBins, Double_t binWidth=1.);
            KTPowerSpectrum(const KTPowerSpectrum& original);
            virtual ~KTPowerSpectrum();

            /// Input should be the direct FFT, not a power spectrum
            virtual void TakeFrequencySpectrum(const KTComplexVector& freqSpect);
            virtual void TakeFrequencySpectrum(unsigned int nBins, const Double_t* real, const Double_t* imag);

            Double_t GetPowerAtFrequency(Double_t freq);
            Double_t GetPhaseAtFrequency(Double_t freq);
            Int_t GetBin(Double_t freq);

            virtual TH1D* CreateMagnitudeHistogram() const;
            virtual TH1D* CreateMagnitudeHistogram(const std::string& name) const;
            virtual TH1D* CreatePhaseHistogram() const;
            virtual TH1D* CreatePhaseHistogram(const std::string& name) const;

            virtual KTPhysicalArray< 1, Double_t >* CreateMagnitudePhysArr() const;
            virtual KTPhysicalArray< 1, Double_t >* CreatePhasePhysArr() const;

            virtual TH1D* CreatePowerDistributionHistogram() const;
            virtual TH1D* CreatePowerDistributionHistogram(const std::string& name) const;

            Double_t GetBinWidth() const;

            void SetBinWidth(Double_t bw);

        protected:
            Double_t fBinWidth;

            ClassDef(KTPowerSpectrum, 2);
    };

    inline Double_t KTPowerSpectrum::GetPowerAtFrequency(Double_t freq)
    {
        return fMagnitude[GetBin(freq)];
    }

    inline Double_t KTPowerSpectrum::GetPhaseAtFrequency(Double_t freq)
    {
        return fPhase[GetBin(freq)];
    }

    inline Int_t KTPowerSpectrum::GetBin(Double_t freq)
    {
        return Int_t(freq / fBinWidth);
    }

    inline Double_t KTPowerSpectrum::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTPowerSpectrum::SetBinWidth(Double_t bw)
    {
        fBinWidth = bw;
        return;
    }
    */

} /* namespace Katydid */
#endif /* KTPOWERSPECTRUM_HH_ */
