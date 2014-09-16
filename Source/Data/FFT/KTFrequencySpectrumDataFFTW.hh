/**
 @file KTFrequencySpectrumDataFFTW.hh
 @brief Contains KTFrequencySpectrumDataFFTW
 @details .
 @author: N. S. Oblath
 @date: Aug 24, 2012
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

            unsigned GetNComponents() const;

            const KTFrequencySpectrumFFTW* GetSpectrumFFTW(unsigned component = 0) const;
            KTFrequencySpectrumFFTW* GetSpectrumFFTW(unsigned component = 0);

            const KTFrequencySpectrum* GetSpectrum(unsigned component = 0) const;
            KTFrequencySpectrum* GetSpectrum(unsigned component = 0);

            void SetSpectrum(KTFrequencySpectrumFFTW* record, unsigned component = 0);

            virtual KTFrequencySpectrumDataFFTWCore& SetNComponents(unsigned channels) = 0;

        protected:
            std::vector< KTFrequencySpectrumFFTW* > fSpectra;

#ifdef ROOT_FOUND
        public:
            TH1D* CreateMagnitudeHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumMag") const;
            TH1D* CreatePhaseHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            TH1D* CreatePowerHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPower") const;

            TH1D* CreatePowerDistributionHistogram(unsigned component = 0, const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

    inline const KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTWCore::GetSpectrumFFTW(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTWCore::GetSpectrumFFTW(unsigned component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencySpectrum* KTFrequencySpectrumDataFFTWCore::GetSpectrum(unsigned component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrum* KTFrequencySpectrumDataFFTWCore::GetSpectrum(unsigned component)
    {
        return fSpectra[component];
    }

    inline unsigned KTFrequencySpectrumDataFFTWCore::GetNComponents() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTFrequencySpectrumDataFFTWCore::SetSpectrum(KTFrequencySpectrumFFTW* record, unsigned component)
    {
        if (component >= fSpectra.size()) SetNComponents(component+1);
        else delete fSpectra[component];
        fSpectra[component] = record;
        return;
    }

#ifdef ROOT_FOUND
    inline TH1D* KTFrequencySpectrumDataFFTWCore::CreateMagnitudeHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreateMagnitudeHistogram(name);
    }
    inline TH1D* KTFrequencySpectrumDataFFTWCore::CreatePhaseHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreatePhaseHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataFFTWCore::CreatePowerHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataFFTWCore::CreatePowerDistributionHistogram(unsigned component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerDistributionHistogram(name);
    }
#endif


    class KTFrequencySpectrumDataFFTW : public KTFrequencySpectrumDataFFTWCore, public KTExtensibleData< KTFrequencySpectrumDataFFTW >
    {
        public:
            KTFrequencySpectrumDataFFTW();
            virtual ~KTFrequencySpectrumDataFFTW();

            virtual KTFrequencySpectrumDataFFTW& SetNComponents(unsigned components);

        private:
            static const std::string sName;

    };

} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATAFFTW_HH_ */
