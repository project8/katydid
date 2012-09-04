/*
 * KTFrequencySpectrum.hh
 *
 *  Created on: Aug 28, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUM_HH_
#define KTFREQUENCYSPECTRUM_HH_

#include "complexpolar.hh"
#include "KTPhysicalArray.hh"

#include "Rtypes.h"

#include <string>

#ifdef ROOT_FOUND
class TH1D;
#endif

namespace Katydid
{
    class KTPowerSpectrum;

    class KTFrequencySpectrum : public KTPhysicalArray< 1, complexpolar< Double_t > >
    {
        public:
            KTFrequencySpectrum();
            KTFrequencySpectrum(size_t nBins, Double_t rangeMin=0., Double_t rangeMax=1.);
            KTFrequencySpectrum(const KTFrequencySpectrum& orig);
            virtual ~KTFrequencySpectrum();

            virtual KTFrequencySpectrum& operator=(const KTFrequencySpectrum& rhs);

            virtual KTFrequencySpectrum& CConjugate();

            virtual KTPowerSpectrum* CreatePowerSpectrum() const;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH1D* CreatePhaseHistogram(const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH1D* CreatePowerHistogram(const std::string& name = "hFrequencySpectrumPower") const;

            virtual TH1D* CreatePowerDistributionHistogram(const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

} /* namespace Katydid */
#endif /* KTFREQUENCYSPECTRUM_HH_ */
