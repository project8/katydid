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
    typedef KTPhysicalArray< 1, KTFrequencySpectrumPolar* > KTMultiFSPolar;

    class KTMultiFSDataPolarCore
    {
        public:
            typedef KTMultiFSPolar multi_spectrum_type;
            typedef KTFrequencySpectrumPolar spectrum_type;

        public:
            KTMultiFSDataPolarCore();
            virtual ~KTMultiFSDataPolarCore();

            const KTMultiFSPolar* GetSpectra(unsigned component = 0) const;
            KTMultiFSPolar* GetSpectra(unsigned component = 0);
            unsigned GetNComponents() const;

            void SetSpectra(KTMultiFSPolar* spectra, unsigned component = 0);
            void SetSpectrum(KTFrequencySpectrumPolar* spectrum, unsigned iSpect, unsigned component = 0);

            void SetNSpectra(unsigned nSpectra);
            virtual KTMultiFSDataPolarCore& SetNComponents(unsigned components) = 0;

        protected:
            void DeleteSpectra(unsigned component = 0);

            std::vector< KTMultiFSPolar* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateMagnitudeHistogram(unsigned component = 0, const std::string& name = "hMultiFSMag") const;
            virtual TH2D* CreatePhaseHistogram(unsigned component = 0, const std::string& name = "hMultiFSPhase") const;

            virtual TH2D* CreatePowerHistogram(unsigned component = 0, const std::string& name = "hMultiFSPower") const;
#endif


    };

    inline const KTMultiFSPolar* KTMultiFSDataPolarCore::GetSpectra(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTMultiFSPolar* KTMultiFSDataPolarCore::GetSpectra(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTMultiFSDataPolarCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTMultiFSDataPolarCore::SetSpectra(KTMultiFSPolar* spectra, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        DeleteSpectra(component);
        fSpectra[component] = spectra;
        return;
    }




    class KTMultiFSDataPolar : public KTMultiFSDataPolarCore, public Nymph::KTExtensibleData< KTMultiFSDataPolar >
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
