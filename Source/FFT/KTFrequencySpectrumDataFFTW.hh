/*
 * KTFrequencySpectrumDataFFTW.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMDATAFFTW_HH_
#define KTFREQUENCYSPECTRUMDATAFFTW_HH_

#include "KTData.hh"

#include "KTFrequencySpectrumFFTW.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <vector>

namespace Katydid
{

    class KTFrequencySpectrumDataFFTWCore
    {
        public:
            KTFrequencySpectrumDataFFTWCore();
            virtual ~KTFrequencySpectrumDataFFTWCore();

            UInt_t GetNComponents() const;

            const KTFrequencySpectrumFFTW* GetSpectrumFFTW(UInt_t component = 0) const;
            KTFrequencySpectrumFFTW* GetSpectrumFFTW(UInt_t component = 0);

            const KTFrequencySpectrum* GetSpectrum(UInt_t component = 0) const;
            KTFrequencySpectrum* GetSpectrum(UInt_t component = 0);

            void SetSpectrum(KTFrequencySpectrumFFTW* record, unsigned component = 0);

            virtual KTFrequencySpectrumDataFFTWCore& SetNComponents(UInt_t channels) = 0;

        protected:
            std::vector< KTFrequencySpectrumFFTW* > fSpectra;

#ifdef ROOT_FOUND
        public:
            TH1D* CreateMagnitudeHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumMag") const;
            TH1D* CreatePhaseHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            TH1D* CreatePowerHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPower") const;

            TH1D* CreatePowerDistributionHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

    inline const KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTWCore::GetSpectrumFFTW(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTWCore::GetSpectrumFFTW(UInt_t component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencySpectrum* KTFrequencySpectrumDataFFTWCore::GetSpectrum(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrum* KTFrequencySpectrumDataFFTWCore::GetSpectrum(UInt_t component)
    {
        return fSpectra[component];
    }

    inline UInt_t KTFrequencySpectrumDataFFTWCore::GetNComponents() const
    {
        return UInt_t(fSpectra.size());
    }

    inline void KTFrequencySpectrumDataFFTWCore::SetSpectrum(KTFrequencySpectrumFFTW* record, UInt_t component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        fSpectra[component] = record;
        return;
    }

#ifdef ROOT_FOUND
    inline TH1D* KTFrequencySpectrumDataFFTWCore::CreateMagnitudeHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreateMagnitudeHistogram(name);
    }
    inline TH1D* KTFrequencySpectrumDataFFTWCore::CreatePhaseHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePhaseHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataFFTWCore::CreatePowerHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataFFTWCore::CreatePowerDistributionHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerDistributionHistogram(name);
    }
#endif


    class KTFrequencySpectrumDataFFTW : public KTFrequencySpectrumDataFFTWCore, public KTExtensibleData< KTFrequencySpectrumDataFFTW >
    {
        public:
            KTFrequencySpectrumDataFFTW();
            virtual ~KTFrequencySpectrumDataFFTW();

            virtual KTFrequencySpectrumDataFFTW& SetNComponents(UInt_t channels);
    };

    inline KTFrequencySpectrumDataFFTW& KTFrequencySpectrumDataFFTW::SetNComponents(UInt_t channels)
    {
        fSpectra.resize(channels);
        return *this;
    }


} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATAFFTW_HH_ */
