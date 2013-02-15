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

    class KTFrequencySpectrumDataFFTW : public KTData< KTFrequencySpectrumFFTW >
    {
        public:
            KTFrequencySpectrumDataFFTW(UInt_t nChannels=1);
            virtual ~KTFrequencySpectrumDataFFTW();

            unsigned GetNComponents() const;

            const KTFrequencySpectrumFFTW* GetSpectrumFFTW(UInt_t component = 0) const;
            KTFrequencySpectrumFFTW* GetSpectrumFFTW(UInt_t component = 0);

            const KTFrequencySpectrum* GetSpectrum(UInt_t component = 0) const;
            KTFrequencySpectrum* GetSpectrum(UInt_t component = 0);

            void SetSpectrum(KTFrequencySpectrumFFTW* record, unsigned component = 0);

            KTFrequencySpectrumDataFFTW& SetNComponents(UInt_t channels);

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

    inline const KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTW::GetSpectrumFFTW(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTW::GetSpectrumFFTW(UInt_t component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencySpectrum* KTFrequencySpectrumDataFFTW::GetSpectrum(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrum* KTFrequencySpectrumDataFFTW::GetSpectrum(UInt_t component)
    {
        return fSpectra[component];
    }

    inline UInt_t KTFrequencySpectrumDataFFTW::GetNComponents() const
    {
        return UInt_t(fSpectra.size());
    }

    inline void KTFrequencySpectrumDataFFTW::SetSpectrum(KTFrequencySpectrumFFTW* record, UInt_t component)
    {
        if (component >= fSpectra.size()) fSpectra.resize(component+1);
        fSpectra[component] = record;
        return;
    }

    inline KTFrequencySpectrumDataFFTW& KTFrequencySpectrumDataFFTW::SetNComponents(UInt_t channels)
    {
        fSpectra.resize(channels);
        return *this;
    }

#ifdef ROOT_FOUND
    inline TH1D* KTFrequencySpectrumDataFFTW::CreateMagnitudeHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreateMagnitudeHistogram(name);
    }
    inline TH1D* KTFrequencySpectrumDataFFTW::CreatePhaseHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePhaseHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataFFTW::CreatePowerHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataFFTW::CreatePowerDistributionHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerDistributionHistogram(name);
    }
#endif


} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATAFFTW_HH_ */
