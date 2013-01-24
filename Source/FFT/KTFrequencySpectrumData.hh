/*
 * KTFrequencySpectrumData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMDATA_HH_
#define KTFREQUENCYSPECTRUMDATA_HH_

#include "KTWriteableData.hh"

#include "KTFrequencySpectrum.hh"

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

            const KTFrequencySpectrum* GetSpectrum(UInt_t channelNum = 0) const;
            KTFrequencySpectrum* GetSpectrum(UInt_t channelNum = 0);
            UInt_t GetNChannels() const;
            Double_t GetTimeInRun() const;


            void SetSpectrum(KTFrequencySpectrum* record, UInt_t channelNum = 0);
            void SetNChannels(UInt_t channels);
            void SetTimeInRun(Double_t tir);

            void Accept(KTWriter* writer) const;

        protected:
            std::vector< KTFrequencySpectrum* > fSpectra;

            Double_t fTimeInRun;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH1D* CreatePhaseHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH1D* CreatePowerHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPower") const;

            virtual TH1D* CreatePowerDistributionHistogram(UInt_t channelNum = 0, const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

    inline const KTFrequencySpectrum* KTFrequencySpectrumData::GetSpectrum(UInt_t channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTFrequencySpectrum* KTFrequencySpectrumData::GetSpectrum(UInt_t channelNum)
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

    inline void KTFrequencySpectrumData::SetSpectrum(KTFrequencySpectrum* record, UInt_t channelNum)
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
