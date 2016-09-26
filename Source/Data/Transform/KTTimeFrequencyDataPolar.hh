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

            unsigned GetNComponents() const;

            const KTTimeFrequencyPolar* GetSpectrumPolar(unsigned component = 0) const;
            KTTimeFrequencyPolar* GetSpectrumPolar(unsigned component = 0);

            const KTTimeFrequency* GetSpectrum(unsigned component = 0) const;
            KTTimeFrequency* GetSpectrum(unsigned component = 0);

            void SetSpectrum(KTTimeFrequencyPolar* record, unsigned component = 0);

            virtual KTTimeFrequencyDataPolarCore& SetNComponents(unsigned channels) = 0;

        protected:
            std::vector< KTTimeFrequencyPolar* > fSpectra;

#ifdef ROOT_FOUND
        public:
            TH2D* CreateMagnitudeHistogram(unsigned component = 0, const std::string& name = "hTimeFrequencyMag") const;
            TH2D* CreatePhaseHistogram(unsigned component = 0, const std::string& name = "hTimeFrequencyPhase") const;

            TH2D* CreatePowerHistogram(unsigned component = 0, const std::string& name = "hTimeFrequencyPower") const;

            //TH1D* CreatePowerDistributionHistogram(unsigned component = 0, const std::string& name = "hTimeFrequencyPowerDist") const;
#endif
    };

    inline const KTTimeFrequencyPolar* KTTimeFrequencyDataPolarCore::GetSpectrumPolar(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTTimeFrequencyPolar* KTTimeFrequencyDataPolarCore::GetSpectrumPolar(unsigned component)
    {
        return fSpectra[component];
    }

    inline const KTTimeFrequency* KTTimeFrequencyDataPolarCore::GetSpectrum(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTTimeFrequency* KTTimeFrequencyDataPolarCore::GetSpectrum(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTTimeFrequencyDataPolarCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTTimeFrequencyDataPolarCore::SetSpectrum(KTTimeFrequencyPolar* record, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fSpectra[component] = record;
        return;
    }

#ifdef ROOT_FOUND
    inline TH2D* KTTimeFrequencyDataPolarCore::CreateMagnitudeHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreateMagnitudeHistogram(name);
    }
    inline TH2D* KTTimeFrequencyDataPolarCore::CreatePhaseHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreatePhaseHistogram(name);
    }
    inline TH2D* KTTimeFrequencyDataPolarCore::CreatePowerHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerHistogram(name);
    }
    /*
    inline TH1D* KTTimeFrequencyDataPolarCore::CreatePowerDistributionHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerDistributionHistogram(name);
    }
    */
#endif





    class KTTimeFrequencyDataPolar : public KTTimeFrequencyDataPolarCore, public Nymph::KTExtensibleData< KTTimeFrequencyDataPolar >
    {
        public:
            KTTimeFrequencyDataPolar();
            virtual ~KTTimeFrequencyDataPolar();

            KTTimeFrequencyDataPolar& SetNComponents(unsigned component);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */

#endif /* KTTIMEFREQUENCYDATAPOLAR_HH_ */
