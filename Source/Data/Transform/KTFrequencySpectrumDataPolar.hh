/*
 * KTFrequencySpectrumDataPolar.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMDATAPOLAR_HH_
#define KTFREQUENCYSPECTRUMDATAPOLAR_HH_

#include "KTData.hh"

#include "KTFrequencySpectrumPolar.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <vector>

namespace Katydid
{

    class KTFrequencySpectrumDataPolarCore
    {
        public:
            KTFrequencySpectrumDataPolarCore();
            virtual ~KTFrequencySpectrumDataPolarCore();

            unsigned GetNComponents() const;

            const KTFrequencySpectrumPolar* GetSpectrumPolar(unsigned component = 0) const;
            KTFrequencySpectrumPolar* GetSpectrumPolar(unsigned component = 0);

            const KTFrequencySpectrum* GetSpectrum(unsigned component = 0) const;
            KTFrequencySpectrum* GetSpectrum(unsigned component = 0);

            void SetSpectrum(KTFrequencySpectrumPolar* record, unsigned component = 0);

            virtual KTFrequencySpectrumDataPolarCore& SetNComponents(unsigned channels) = 0;

        protected:
            std::vector< KTFrequencySpectrumPolar* > fSpectra;
/*
#ifdef ROOT_FOUND
        public:
            TH1D* CreateMagnitudeHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumMag") const;
            TH1D* CreatePhaseHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            TH1D* CreatePowerHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPower") const;

            TH1D* CreatePowerDistributionHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
*/
    };

    inline const KTFrequencySpectrumPolar* KTFrequencySpectrumDataPolarCore::GetSpectrumPolar(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrumPolar* KTFrequencySpectrumDataPolarCore::GetSpectrumPolar(unsigned component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencySpectrum* KTFrequencySpectrumDataPolarCore::GetSpectrum(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrum* KTFrequencySpectrumDataPolarCore::GetSpectrum(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTFrequencySpectrumDataPolarCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTFrequencySpectrumDataPolarCore::SetSpectrum(KTFrequencySpectrumPolar* record, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        else delete fSpectra[component];
        fSpectra[component] = record;
        return;
    }

    /*
#ifdef ROOT_FOUND
    inline TH1D* KTFrequencySpectrumDataPolarCore::CreateMagnitudeHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreateMagnitudeHistogram(name);
    }
    inline TH1D* KTFrequencySpectrumDataPolarCore::CreatePhaseHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreatePhaseHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataPolarCore::CreatePowerHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataPolarCore::CreatePowerDistributionHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerDistributionHistogram(name);
    }
#endif
*/




    class KTFrequencySpectrumDataPolar : public KTFrequencySpectrumDataPolarCore, public KTExtensibleData< KTFrequencySpectrumDataPolar >
    {
        public:
            KTFrequencySpectrumDataPolar();
            virtual ~KTFrequencySpectrumDataPolar();

            KTFrequencySpectrumDataPolar& SetNComponents(unsigned component);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATAPOLAR_HH_ */
