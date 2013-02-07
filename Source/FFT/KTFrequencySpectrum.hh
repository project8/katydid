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

            virtual KTFrequencySpectrum& CConjugate() = 0;

            virtual KTPowerSpectrum* CreatePowerSpectrum() const= 0;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(const std::string& name = "hFrequencySpectrumMag") const = 0;
            virtual TH1D* CreatePhaseHistogram(const std::string& name = "hFrequencySpectrumPhase") const = 0;

            virtual TH1D* CreatePowerHistogram(const std::string& name = "hFrequencySpectrumPower") const = 0;

            virtual TH1D* CreatePowerDistributionHistogram(const std::string& name = "hFrequencySpectrumPowerDist") const = 0;
#endif
    };

} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUM_HH_ */
