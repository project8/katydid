/*
 * KTMultiFSDataPolarCore.hh
 *
 *  Created on: May 20, 2013
 *      Author: nsoblath
 */

#ifndef KTMULTIFSDATAPOLAR_HH_
#define KTMULTIFSDATAPOLAR_HH_

#include "KTData.hh"

#include "KTFrequencySpectrumPolar.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#endif

#include <vector>

namespace Katydid
{
    using namespace Nymph;

    class KTMultiFSDataPolarCore
    {
        public:
            KTMultiFSDataPolarCore();
            virtual ~KTMultiFSDataPolarCore();

            const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* GetSpectra(unsigned component = 0) const;
            KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* GetSpectra(unsigned component = 0);
            unsigned GetNComponents() const;

            void SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* spectra, unsigned component = 0);
            void SetSpectrum(KTFrequencySpectrumPolar* spectrum, unsigned iSpect, unsigned component = 0);

            void SetNSpectra(unsigned nSpectra);
            virtual KTMultiFSDataPolarCore& SetNComponents(unsigned components) = 0;

        protected:
            void DeleteSpectra(unsigned component = 0);

            std::vector< KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateMagnitudeHistogram(unsigned component = 0, const std::string& name = "hMultiFSMag") const;
            virtual TH2D* CreatePhaseHistogram(unsigned component = 0, const std::string& name = "hMultiFSPhase") const;

            virtual TH2D* CreatePowerHistogram(unsigned component = 0, const std::string& name = "hMultiFSPower") const;
#endif


    };

    inline const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* KTMultiFSDataPolarCore::GetSpectra(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* KTMultiFSDataPolarCore::GetSpectra(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTMultiFSDataPolarCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTMultiFSDataPolarCore::SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* spectra, unsigned component)
    {
        if (component >= fSpectra.size())
            SetNComponents(component+1);
        DeleteSpectra(component);
        fSpectra[component] = spectra;
        return;
    }

    inline void KTMultiFSDataPolarCore::SetSpectrum(KTFrequencySpectrumPolar* spectrum, unsigned iSpect, unsigned component)
    {
        if (component >= fSpectra.size())
            SetNComponents(component+1);
        if (fSpectra[component] == NULL)
            fSpectra[component] = new KTPhysicalArray< 1, KTFrequencySpectrumPolar* >();
        (*fSpectra[component])(iSpect) = spectrum;
        return;
    }

    inline void KTMultiFSDataPolarCore::DeleteSpectra(unsigned component)
    {
        if (component >= fSpectra.size())
            return;
        for (KTPhysicalArray< 1, KTFrequencySpectrumPolar* >::iterator iter = fSpectra[component]->begin(); iter != fSpectra[component]->end(); iter++)
        {
            delete *iter;
        }
        delete fSpectra[component];
        fSpectra[component] = NULL;
        return;
    }




    class KTMultiFSDataPolar : public KTMultiFSDataPolarCore, public KTExtensibleData< KTMultiFSDataPolar >
    {
        public:
            KTMultiFSDataPolar();
            virtual ~KTMultiFSDataPolar();

            KTMultiFSDataPolar& SetNComponents(unsigned component);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */

#endif /* KTMULTIFSDATAPOLAR_HH_ */
