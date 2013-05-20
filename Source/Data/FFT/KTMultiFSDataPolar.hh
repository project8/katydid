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

    class KTMultiFSDataPolarCore
    {
        public:
            KTMultiFSDataPolarCore();
            virtual ~KTMultiFSDataPolarCore();

            const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* GetSpectra(UInt_t component = 0) const;
            KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* GetSpectra(UInt_t component = 0);
            UInt_t GetNComponents() const;

            void SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* spectra, UInt_t component = 0);
            void SetSpectrum(KTFrequencySpectrumPolar* spectrum, UInt_t iSpect, UInt_t component = 0);

            void SetNSpectra(UInt_t nSpectra);
            virtual KTMultiFSDataPolarCore& SetNComponents(UInt_t components) = 0;

        protected:
            void DeleteSpectra(UInt_t component = 0);

            std::vector< KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateMagnitudeHistogram(UInt_t component = 0, const std::string& name = "hMultiFSMag") const;
            virtual TH2D* CreatePhaseHistogram(UInt_t component = 0, const std::string& name = "hMultiFSPhase") const;

            virtual TH2D* CreatePowerHistogram(UInt_t component = 0, const std::string& name = "hMultiFSPower") const;
#endif


    };

    inline const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* KTMultiFSDataPolarCore::GetSpectra(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* KTMultiFSDataPolarCore::GetSpectra(UInt_t component)
    {
        return fSpectra[component];
    }

    inline UInt_t KTMultiFSDataPolarCore::GetNComponents() const
    {
        return UInt_t(fSpectra.size());
    }

    inline void KTMultiFSDataPolarCore::SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* spectra, UInt_t component)
    {
        if (component >= fSpectra.size())
            SetNComponents(component+1);
        DeleteSpectra[component];
        fSpectra[component] = spectra;
        return;
    }

    inline void KTMultiFSDataPolarCore::SetSpectrum(KTFrequencySpectrumPolar* spectrum, UInt_t iSpect, UInt_t component)
    {
        if (component >= fSpectra.size())
            SetNComponents(component+1);
        if (fSpectra[component] == NULL)
            fSpectra[component] = new KTPhysicalArray< 1, KTFrequencySpectrumPolar* >();
        (*fSpectra[component])(iSpect) = spectrum;
        return;
    }

    inline void KTMultiFSDataPolarCore::DeleteSpectra(UInt_t component)
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

            KTMultiFSDataPolar& SetNComponents(UInt_t component);
    };

    inline KTMultiFSDataPolar& KTMultiFSDataPolar::SetNComponents(UInt_t components)
    {
        UInt_t oldSize = fSpectra.size();
        if (components < oldSize)
        {
            for (UInt_t iComponent = components; iComponent < oldSize; iComponent++)
            {
                DeleteSpectra[iComponent];
            }
        }

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

#endif /* KTMULTIFSDATAPOLAR_HH_ */
