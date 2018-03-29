/*
 * KTMultiPSDataCore.hh
 *
 *  Created on: Aug 31, 2017
 *      Author: nsoblath
 */

#ifndef KTMULTIPSDATA_HH_
#define KTMULTIPSDATA_HH_

#include "KTData.hh"

#include "KTPowerSpectrum.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#endif

#include <vector>

namespace Katydid
{
    typedef KTPhysicalArray< 1, KTPowerSpectrum* > KTMultiPS;

    class KTMultiPSDataCore
    {
        public:
            typedef KTMultiPS multi_spectrum_type;
            typedef KTPowerSpectrum spectrum_type;

        public:
            KTMultiPSDataCore();
            virtual ~KTMultiPSDataCore();

            const KTMultiPS* GetSpectra(unsigned component = 0) const;
            KTMultiPS* GetSpectra(unsigned component = 0);
            unsigned GetNComponents() const;

            void SetSpectra(KTMultiPS* spectra, unsigned component = 0);
            void SetSpectrum(KTPowerSpectrum* spectrum, unsigned iSpect, unsigned component = 0);

            void SetNSpectra(unsigned nSpectra);
            virtual KTMultiPSDataCore& SetNComponents(unsigned components) = 0;

        protected:
            void DeleteSpectra(unsigned component = 0);

            std::vector< KTMultiPS* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreatePowerHistogram(unsigned component = 0, const std::string& name = "hMultiPS") const;
#endif

    };

    inline const KTMultiPS* KTMultiPSDataCore::GetSpectra(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTMultiPS* KTMultiPSDataCore::GetSpectra(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTMultiPSDataCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTMultiPSDataCore::SetSpectra(KTMultiPS* spectra, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        DeleteSpectra(component);
        fSpectra[component] = spectra;
        return;
    }



    class KTMultiPSData : public KTMultiPSDataCore, public Nymph::KTExtensibleData< KTMultiPSData >
    {
        public:
            KTMultiPSData();
            virtual ~KTMultiPSData();

            KTMultiPSData& SetNComponents(unsigned component);

        public:
            static const std::string sName;

    };

} /* namespace Katydid */

#endif /* KTMULTIPSDATA_HH_ */
