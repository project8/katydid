/*
 * KTFrequencySpectrum.hh
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUM_HH_
#define KTFREQUENCYSPECTRUM_HH_

#include "Rtypes.h"

#include <string>

#ifdef ROOT_FOUND
class TH1D;
#endif

namespace Katydid
{
    class KTPowerSpectrum;

    class KTFrequencySpectrum
    {
        public:
            KTFrequencySpectrum();
            virtual ~KTFrequencySpectrum();

            /// Get the size of the array using the KTFrequencySpectrum interface
            virtual UInt_t GetNFrequencyBins() const = 0;

            /// Get the bin width of the array using the KTFrequencySpectrum interface
            virtual Double_t GetFrequencyBinWidth() const = 0;

            virtual Double_t GetReal(UInt_t bin) const = 0;
            virtual Double_t GetImag(UInt_t bin) const = 0;

            virtual void SetRect(UInt_t bin, Double_t real, Double_t imag) = 0;

            virtual Double_t GetAbs(UInt_t bin) const = 0;
            virtual Double_t GetArg(UInt_t bin) const = 0;

            virtual void SetPolar(UInt_t bin, Double_t abs, Double_t arg) = 0;

            virtual KTFrequencySpectrum& CConjugate() = 0;

            virtual KTPowerSpectrum* CreatePowerSpectrum() const= 0;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(const std::string& name = "hFrequencySpectrumMag") const = 0;
            virtual TH1D* CreatePhaseHistogram(const std::string& name = "hFrequencySpectrumPhase") const = 0;

            virtual TH1D* CreatePowerHistogram(const std::string& name = "hFrequencySpectrumPower") const = 0;

            virtual TH1D* CreateMagnitudeDistributionHistogram(const std::string& name = "hFrequencySpectrumMagDist") const = 0;
            virtual TH1D* CreatePowerDistributionHistogram(const std::string& name = "hFrequencySpectrumPowerDist") const = 0;
#endif
    };

} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUM_HH_ */
