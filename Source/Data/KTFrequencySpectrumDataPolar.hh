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

            UInt_t GetNComponents() const;

            const KTFrequencySpectrumPolar* GetSpectrumPolar(UInt_t component = 0) const;
            KTFrequencySpectrumPolar* GetSpectrumPolar(UInt_t component = 0);

            const KTFrequencySpectrum* GetSpectrum(UInt_t component = 0) const;
            KTFrequencySpectrum* GetSpectrum(UInt_t component = 0);

            void SetSpectrum(KTFrequencySpectrumPolar* record, UInt_t component = 0);

            virtual KTFrequencySpectrumDataPolarCore& SetNComponents(UInt_t channels) = 0;

        protected:
            std::vector< KTFrequencySpectrumPolar* > fSpectra;

#ifdef ROOT_FOUND
        public:
            TH1D* CreateMagnitudeHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumMag") const;
            TH1D* CreatePhaseHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            TH1D* CreatePowerHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPower") const;

            TH1D* CreatePowerDistributionHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

    inline const KTFrequencySpectrumPolar* KTFrequencySpectrumDataPolarCore::GetSpectrumPolar(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrumPolar* KTFrequencySpectrumDataPolarCore::GetSpectrumPolar(UInt_t component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencySpectrum* KTFrequencySpectrumDataPolarCore::GetSpectrum(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrum* KTFrequencySpectrumDataPolarCore::GetSpectrum(UInt_t component)
    {
        return fSpectra[component];
    }

    inline UInt_t KTFrequencySpectrumDataPolarCore::GetNComponents() const
    {
        return UInt_t(fSpectra.size());
    }

    inline void KTFrequencySpectrumDataPolarCore::SetSpectrum(KTFrequencySpectrumPolar* record, UInt_t component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fSpectra[component] = record;
        return;
    }

#ifdef ROOT_FOUND
    inline TH1D* KTFrequencySpectrumDataPolarCore::CreateMagnitudeHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreateMagnitudeHistogram(name);
    }
    inline TH1D* KTFrequencySpectrumDataPolarCore::CreatePhaseHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePhaseHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataPolarCore::CreatePowerHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataPolarCore::CreatePowerDistributionHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerDistributionHistogram(name);
    }
#endif





    class KTFrequencySpectrumDataPolar : public KTFrequencySpectrumDataPolarCore, public KTExtensibleData< KTFrequencySpectrumDataPolar >
    {
        public:
            KTFrequencySpectrumDataPolar();
            virtual ~KTFrequencySpectrumDataPolar();

            KTFrequencySpectrumDataPolar& SetNComponents(UInt_t component);
    };

    inline KTFrequencySpectrumDataPolar& KTFrequencySpectrumDataPolar::SetNComponents(UInt_t components)
    {
        UInt_t oldSize = fSpectra.size();
        fSpectra.resize(components);
        if (components > oldSize)
        {
            for (UInt_t iComponent = oldSize; iComponent < components; iComponent++)
            {
                fSpectra[iComponent] = NULL;
            }
        }
        return *this;
    }


} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATAPOLAR_HH_ */
