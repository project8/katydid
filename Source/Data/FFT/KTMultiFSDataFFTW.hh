/*
 * KTMultiFSDataFFTWCore.hh
 *
 *  Created on: May 21, 2013
 *      Author: nsoblath
 */

#ifndef KTMULTIFSDATAFFTW_HH_
#define KTMULTIFSDATAFFTW_HH_

#include "KTData.hh"

#include "KTFrequencySpectrumFFTW.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#endif

#include <vector>

namespace Katydid
{

    class KTMultiFSDataFFTWCore
    {
        public:
            KTMultiFSDataFFTWCore();
            virtual ~KTMultiFSDataFFTWCore();

            const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* GetSpectra(UInt_t component = 0) const;
            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* GetSpectra(UInt_t component = 0);
            UInt_t GetNComponents() const;

            void SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* spectra, UInt_t component = 0);
            void SetSpectrum(KTFrequencySpectrumFFTW* spectrum, UInt_t iSpect, UInt_t component = 0);

            void SetNSpectra(UInt_t nSpectra);
            virtual KTMultiFSDataFFTWCore& SetNComponents(UInt_t components) = 0;

        protected:
            void DeleteSpectra(UInt_t component = 0);

            std::vector< KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateMagnitudeHistogram(UInt_t component = 0, const std::string& name = "hMultiFSMag") const;
            virtual TH2D* CreatePhaseHistogram(UInt_t component = 0, const std::string& name = "hMultiFSPhase") const;

            virtual TH2D* CreatePowerHistogram(UInt_t component = 0, const std::string& name = "hMultiFSPower") const;
#endif


    };

    inline const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* KTMultiFSDataFFTWCore::GetSpectra(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* KTMultiFSDataFFTWCore::GetSpectra(UInt_t component)
    {
        return fSpectra[component];
    }

    inline UInt_t KTMultiFSDataFFTWCore::GetNComponents() const
    {
        return UInt_t(fSpectra.size());
    }

    inline void KTMultiFSDataFFTWCore::SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* spectra, UInt_t component)
    {
        if (component >= fSpectra.size())
            SetNComponents(component+1);
        DeleteSpectra(component);
        fSpectra[component] = spectra;
        return;
    }

    inline void KTMultiFSDataFFTWCore::SetSpectrum(KTFrequencySpectrumFFTW* spectrum, UInt_t iSpect, UInt_t component)
    {
        if (component >= fSpectra.size())
            SetNComponents(component+1);
        if (fSpectra[component] == NULL)
            fSpectra[component] = new KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >();
        (*fSpectra[component])(iSpect) = spectrum;
        return;
    }

    inline void KTMultiFSDataFFTWCore::DeleteSpectra(UInt_t component)
    {
        if (component >= fSpectra.size())
            return;
        for (KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >::iterator iter = fSpectra[component]->begin(); iter != fSpectra[component]->end(); iter++)
        {
            delete *iter;
        }
        delete fSpectra[component];
        fSpectra[component] = NULL;
        return;
    }




    class KTMultiFSDataFFTW : public KTMultiFSDataFFTWCore, public KTExtensibleData< KTMultiFSDataFFTW >
    {
        public:
        KTMultiFSDataFFTW();
            virtual ~KTMultiFSDataFFTW();

            KTMultiFSDataFFTW& SetNComponents(UInt_t component);
    };

} /* namespace Katydid */

#endif /* KTMULTIFSDATAFFTW_HH_ */
