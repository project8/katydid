/*
 * KTFrequencySpectrumDataFFTW.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMDATAFFTW_HH_
#define KTFREQUENCYSPECTRUMDATAFFTW_HH_

#include "KTFrequencySpectrumData.hh"

#include "KTFrequencySpectrumFFTW.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <vector>

namespace Katydid
{

    class KTFrequencySpectrumDataFFTW : public KTFrequencySpectrumData
    {
        public:
            KTFrequencySpectrumDataFFTW(UInt_t nChannels=1);
            virtual ~KTFrequencySpectrumDataFFTW();

            const KTFrequencySpectrumFFTW* GetSpectrumFFTW(UInt_t component = 0) const;
            KTFrequencySpectrumFFTW* GetSpectrumFFTW(UInt_t component = 0);

            virtual const KTFrequencySpectrum* GetSpectrum(UInt_t component = 0) const;
            virtual KTFrequencySpectrum* GetSpectrum(UInt_t component = 0);
            virtual unsigned GetNComponents() const;
            virtual Double_t GetTimeInRun() const;
            virtual Double_t GetTimeLength() const;
            virtual ULong64_t GetSliceNumber() const;

            void SetSpectrum(KTFrequencySpectrumFFTW* record, unsigned component = 0);

            virtual void SetNComponents(UInt_t channels);
            virtual void SetTimeInRun(Double_t tir);
            virtual void SetTimeLength(Double_t length);
            virtual void SetSliceNumber(ULong64_t slice);

            virtual void Accept(KTWriter* writer) const;

        protected:
            std::vector< KTFrequencySpectrumFFTW* > fSpectra;

            Double_t fTimeInRun;
            Double_t fTimeLength;
            ULong64_t fSliceNumber;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH1D* CreatePhaseHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH1D* CreatePowerHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPower") const;

            virtual TH1D* CreatePowerDistributionHistogram(UInt_t component = 0, const std::string& name = "hFrequencySpectrumPowerDist") const;
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

    inline Double_t KTFrequencySpectrumDataFFTW::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline Double_t KTFrequencySpectrumDataFFTW::GetTimeLength() const
    {
        return fTimeLength;
    }

    inline ULong64_t KTFrequencySpectrumDataFFTW::GetSliceNumber() const
    {
        return fSliceNumber;
    }

    inline void KTFrequencySpectrumDataFFTW::SetSpectrum(KTFrequencySpectrumFFTW* record, UInt_t component)
    {
        if (component >= fSpectra.size()) fSpectra.resize(component+1);
        fSpectra[component] = record;
    }

    inline void KTFrequencySpectrumDataFFTW::SetNComponents(UInt_t channels)
    {
        fSpectra.resize(channels);
        return;
    }

    inline void KTFrequencySpectrumDataFFTW::SetTimeInRun(Double_t tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTFrequencySpectrumDataFFTW::SetTimeLength(Double_t length)
    {
        fTimeLength = length;
        return;
    }

    inline void KTFrequencySpectrumDataFFTW::SetSliceNumber(ULong64_t slice)
    {
        fSliceNumber = slice;
        return;
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
