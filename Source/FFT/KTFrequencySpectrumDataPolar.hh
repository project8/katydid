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

            const KTFrequencySpectrumPolar* GetSpectrumPolar(UInt_t channelNum = 0) const;
            KTFrequencySpectrumPolar* GetSpectrumPolar(UInt_t channelNum = 0);

            virtual const KTFrequencySpectrum* GetSpectrum(UInt_t channelNum = 0) const;
            virtual KTFrequencySpectrum* GetSpectrum(UInt_t channelNum = 0);
            virtual UInt_t GetNChannels() const;
            virtual Double_t GetTimeInRun() const;
            virtual Double_t GetTimeLength() const;
            virtual ULong64_t GetSliceNumber() const;

            void SetSpectrum(KTFrequencySpectrumPolar* record, UInt_t channelNum = 0);

            virtual void SetNChannels(UInt_t channels);
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
            virtual TH1D* CreateMagnitudeHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH1D* CreatePhaseHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH1D* CreatePowerHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPower") const;

            virtual TH1D* CreatePowerDistributionHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

    inline const KTFrequencySpectrumPolar* KTFrequencySpectrumDataPolar::GetSpectrumPolar(UInt_t channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTFrequencySpectrumPolar* KTFrequencySpectrumDataPolar::GetSpectrumPolar(UInt_t channelNum)
    {
        return fSpectra[channelNum];
    }

    inline const KTFrequencySpectrum* KTFrequencySpectrumDataPolar::GetSpectrum(UInt_t channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTFrequencySpectrum* KTFrequencySpectrumDataPolar::GetSpectrum(UInt_t channelNum)
    {
        return fSpectra[channelNum];
    }

    inline UInt_t KTFrequencySpectrumDataPolar::GetNChannels() const
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

    inline void KTFrequencySpectrumDataPolar::SetSpectrum(KTFrequencySpectrumPolar* record, UInt_t channelNum)
    {
        if (channelNum >= fSpectra.size()) fSpectra.resize(channelNum+1);
        fSpectra[channelNum] = record;
    }

    inline void KTFrequencySpectrumDataPolar::SetNChannels(UInt_t channels)
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
    inline TH1D* KTFrequencySpectrumDataPolar::CreateMagnitudeHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreateMagnitudeHistogram(name);
    }
    inline TH1D* KTFrequencySpectrumDataPolar::CreatePhaseHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePhaseHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataPolar::CreatePowerHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePowerHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataPolar::CreatePowerDistributionHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePowerDistributionHistogram(name);
    }
#endif


} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATAPOLAR_HH_ */
