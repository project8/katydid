/*
 * KTTimeFrequencyDataPolar.hh
 *
 *  Created on: May 16, 2013
 *      Author: nsoblath
 */

#ifndef KTTIMEFREQUENCYDATAPOLAR_HH_
#define KTTIMEFREQUENCYDATAPOLAR_HH_

#include "KTData.hh"

#include "KTTimeFrequencyPolar.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#endif

#include <vector>

namespace Katydid
{

    class KTTimeFrequencyDataPolarCore
    {
        public:
            KTTimeFrequencyDataPolarCore();
            virtual ~KTTimeFrequencyDataPolarCore();

            UInt_t GetNComponents() const;

            const KTTimeFrequencyPolar* GetSpectrumPolar(UInt_t component = 0) const;
            KTTimeFrequencyPolar* GetSpectrumPolar(UInt_t component = 0);

            const KTTimeFrequency* GetSpectrum(UInt_t component = 0) const;
            KTTimeFrequency* GetSpectrum(UInt_t component = 0);

            void SetSpectrum(KTTimeFrequencyPolar* record, UInt_t component = 0);

            virtual KTTimeFrequencyDataPolarCore& SetNComponents(UInt_t channels) = 0;

        protected:
            std::vector< KTTimeFrequencyPolar* > fSpectra;

#ifdef ROOT_FOUND
        public:
            TH2D* CreateMagnitudeHistogram(UInt_t component = 0, const std::string& name = "hTimeFrequencyMag") const;
            TH2D* CreatePhaseHistogram(UInt_t component = 0, const std::string& name = "hTimeFrequencyPhase") const;

            TH2D* CreatePowerHistogram(UInt_t component = 0, const std::string& name = "hTimeFrequencyPower") const;

            //TH1D* CreatePowerDistributionHistogram(UInt_t component = 0, const std::string& name = "hTimeFrequencyPowerDist") const;
#endif
    };

    inline const KTTimeFrequencyPolar* KTTimeFrequencyDataPolarCore::GetSpectrumPolar(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTTimeFrequencyPolar* KTTimeFrequencyDataPolarCore::GetSpectrumPolar(UInt_t component)
    {
        return fSpectra[component];
    }

    inline const KTTimeFrequency* KTTimeFrequencyDataPolarCore::GetSpectrum(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTTimeFrequency* KTTimeFrequencyDataPolarCore::GetSpectrum(UInt_t component)
    {
        return fSpectra[component];
    }

    inline UInt_t KTTimeFrequencyDataPolarCore::GetNComponents() const
    {
        return UInt_t(fSpectra.size());
    }

    inline void KTTimeFrequencyDataPolarCore::SetSpectrum(KTTimeFrequencyPolar* record, UInt_t component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fSpectra[component] = record;
        return;
    }

#ifdef ROOT_FOUND
    inline TH2D* KTTimeFrequencyDataPolarCore::CreateMagnitudeHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreateMagnitudeHistogram(name);
    }
    inline TH2D* KTTimeFrequencyDataPolarCore::CreatePhaseHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePhaseHistogram(name);
    }
    inline TH2D* KTTimeFrequencyDataPolarCore::CreatePowerHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerHistogram(name);
    }
    /*
    inline TH1D* KTTimeFrequencyDataPolarCore::CreatePowerDistributionHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerDistributionHistogram(name);
    }
    */
#endif





    class KTTimeFrequencyDataPolar : public KTTimeFrequencyDataPolarCore, public KTExtensibleData< KTTimeFrequencyDataPolar >
    {
        public:
            KTTimeFrequencyDataPolar();
            virtual ~KTTimeFrequencyDataPolar();

            KTTimeFrequencyDataPolar& SetNComponents(UInt_t component);
    };

} /* namespace Katydid */

#endif /* KTTIMEFREQUENCYDATAPOLAR_HH_ */
