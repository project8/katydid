/*
 * KTFrequencySpectrumDataPolar.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMDATAPOLAR_HH_
#define KTFREQUENCYSPECTRUMDATAPOLAR_HH_

#include "KTFrequencySpectrumData.hh"

#include "KTFrequencySpectrumPolar.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <vector>

namespace Katydid
{

    class KTFrequencySpectrumDataPolar : public KTFrequencySpectrumData
    {
        public:
            KTFrequencySpectrumDataPolar(UInt_t nChannels=1);
            virtual ~KTFrequencySpectrumDataPolar();

            const KTFrequencySpectrumPolar* GetSpectrumPolar(UInt_t component = 0) const;
            KTFrequencySpectrumPolar* GetSpectrumPolar(UInt_t component = 0);

            virtual const KTFrequencySpectrum* GetSpectrum(UInt_t component = 0) const;
            virtual KTFrequencySpectrum* GetSpectrum(UInt_t component = 0);
            virtual UInt_t GetNComponents() const;
            virtual Double_t GetTimeInRun() const;
            virtual Double_t GetTimeLength() const;
            virtual ULong64_t GetSliceNumber() const;

            void SetSpectrum(KTFrequencySpectrumPolar* record, UInt_t component = 0);

            virtual void SetNComponents(UInt_t channels);
            virtual void SetTimeInRun(Double_t tir);
            virtual void SetTimeLength(Double_t length);
            virtual void SetSliceNumber(ULong64_t slice);

            virtual void Accept(KTWriter* writer) const;

        protected:
            std::vector< KTFrequencySpectrumPolar* > fSpectra;

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

    inline const KTFrequencySpectrumPolar* KTFrequencySpectrumDataPolar::GetSpectrumPolar(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrumPolar* KTFrequencySpectrumDataPolar::GetSpectrumPolar(UInt_t component)
    {
        return fSpectra[component];
    }

    inline const KTFrequencySpectrum* KTFrequencySpectrumDataPolar::GetSpectrum(UInt_t component) const
    {
        return fSpectra[component];
    }

    inline KTFrequencySpectrum* KTFrequencySpectrumDataPolar::GetSpectrum(UInt_t component)
    {
        return fSpectra[component];
    }

    inline UInt_t KTFrequencySpectrumDataPolar::GetNComponents() const
    {
        return UInt_t(fSpectra.size());
    }

    inline Double_t KTFrequencySpectrumDataPolar::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline Double_t KTFrequencySpectrumDataPolar::GetTimeLength() const
    {
        return fTimeLength;
    }

    inline ULong64_t KTFrequencySpectrumDataPolar::GetSliceNumber() const
    {
        return fSliceNumber;
    }

    inline void KTFrequencySpectrumDataPolar::SetSpectrum(KTFrequencySpectrumPolar* record, UInt_t component)
    {
        if (component >= fSpectra.size()) fSpectra.resize(component+1);
        fSpectra[component] = record;
    }

    inline void KTFrequencySpectrumDataPolar::SetNComponents(UInt_t channels)
    {
        fSpectra.resize(channels);
        return;
    }

    inline void KTFrequencySpectrumDataPolar::SetTimeInRun(Double_t tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTFrequencySpectrumDataPolar::SetTimeLength(Double_t length)
    {
        fTimeLength = length;
        return;
    }

    inline void KTFrequencySpectrumDataPolar::SetSliceNumber(ULong64_t slice)
    {
        fSliceNumber = slice;
        return;
    }

#ifdef ROOT_FOUND
    inline TH1D* KTFrequencySpectrumDataPolar::CreateMagnitudeHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreateMagnitudeHistogram(name);
    }
    inline TH1D* KTFrequencySpectrumDataPolar::CreatePhaseHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePhaseHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataPolar::CreatePowerHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataPolar::CreatePowerDistributionHistogram(UInt_t component, const std::string& name) const
    {
        return fSpectra[component]->CreatePowerDistributionHistogram(name);
    }
#endif


} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATAPOLAR_HH_ */
