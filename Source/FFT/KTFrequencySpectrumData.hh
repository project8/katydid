/*
 * KTFrequencySpectrumData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMDATA_HH_
#define KTFREQUENCYSPECTRUMDATA_HH_

#include "KTWriteableData.hh"

#include "KTFrequencySpectrumPolar.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <vector>

namespace Katydid
{

    class KTFrequencySpectrumData : public KTWriteableData
    {
        public:
            KTFrequencySpectrumData(unsigned nChannels=1);
            virtual ~KTFrequencySpectrumData();

            const KTFrequencySpectrumPolar* GetSpectrum(UInt_t channelNum = 0) const;
            KTFrequencySpectrumPolar* GetSpectrum(UInt_t channelNum = 0);
            UInt_t GetNChannels() const;
            Double_t GetTimeInRun() const;
            ULong64_t GetSliceNumber() const;

            void SetSpectrum(KTFrequencySpectrumPolar* record, UInt_t channelNum = 0);
            void SetNChannels(UInt_t channels);
            void SetTimeInRun(Double_t tir);
            void SetSliceNumber(ULong64_t slice);

            void Accept(KTWriter* writer) const;

        protected:
            std::vector< KTFrequencySpectrumPolar* > fSpectra;

            Double_t fTimeInRun;
            ULong64_t fSliceNumber;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH1D* CreatePhaseHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH1D* CreatePowerHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPower") const;

            virtual TH1D* CreatePowerDistributionHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

    inline const KTFrequencySpectrumPolar* KTFrequencySpectrumData::GetSpectrum(UInt_t channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTFrequencySpectrumPolar* KTFrequencySpectrumData::GetSpectrum(UInt_t channelNum)
    {
        return fSpectra[channelNum];
    }

    inline UInt_t KTFrequencySpectrumData::GetNChannels() const
    {
        return UInt_t(fSpectra.size());
    }

    inline Double_t KTFrequencySpectrumData::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline ULong64_t KTFrequencySpectrumData::GetSliceNumber() const
    {
        return fSliceNumber;
    }

    inline void KTFrequencySpectrumData::SetSpectrum(KTFrequencySpectrumPolar* record, UInt_t channelNum)
    {
        if (channelNum >= fSpectra.size()) fSpectra.resize(channelNum+1);
        fSpectra[channelNum] = record;
    }

    inline void KTFrequencySpectrumData::SetNChannels(UInt_t channels)
    {
        fSpectra.resize(channels);
        return;
    }

    inline void KTFrequencySpectrumData::SetTimeInRun(Double_t tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTFrequencySpectrumData::SetSliceNumber(ULong64_t slice)
    {
        fSliceNumber = slice;
        return;
    }

#ifdef ROOT_FOUND
    inline TH1D* KTFrequencySpectrumData::CreateMagnitudeHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreateMagnitudeHistogram(name);
    }
    inline TH1D* KTFrequencySpectrumData::CreatePhaseHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePhaseHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumData::CreatePowerHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePowerHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumData::CreatePowerDistributionHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePowerDistributionHistogram(name);
    }
#endif


} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATA_HH_ */
