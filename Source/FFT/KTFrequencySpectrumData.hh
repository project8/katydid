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

            const std::string& GetName() const;
            static const std::string& StaticGetName();

            const KTFrequencySpectrum* GetSpectrum(unsigned channelNum = 0) const;
            KTFrequencySpectrum* GetSpectrum(unsigned channelNum = 0);
            unsigned GetNChannels() const;

            void SetSpectrum(KTFrequencySpectrum* record, unsigned channelNum = 0);
            void SetNChannels(unsigned channels);

            void Accept(KTPublisher* publisher) const;

        protected:
            static std::string fName;

            std::vector< KTFrequencySpectrum* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH1D* CreatePhaseHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH1D* CreatePowerHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPower") const;

            virtual TH1D* CreatePowerDistributionHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

    inline const std::string& KTFrequencySpectrumData::GetName() const
    {
        return fName;
    }

    inline const KTFrequencySpectrum* KTFrequencySpectrumData::GetSpectrum(unsigned channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTFrequencySpectrum* KTFrequencySpectrumData::GetSpectrum(unsigned channelNum)
    {
        return fSpectra[channelNum];
    }

    inline unsigned KTFrequencySpectrumData::GetNChannels() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTFrequencySpectrumData::SetSpectrum(KTFrequencySpectrum* record, unsigned channelNum)
    {
        if (channelNum >= fSpectra.size()) fSpectra.resize(channelNum+1);
        fSpectra[channelNum] = record;
    }

    inline void KTFrequencySpectrumData::SetNChannels(unsigned channels)
    {
        fSpectra.resize(channels);
        return;
    }

#ifdef ROOT_FOUND
    inline TH1D* KTFrequencySpectrumData::CreateMagnitudeHistogram(unsigned channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreateMagnitudeHistogram(name);
    }
    inline TH1D* KTFrequencySpectrumData::CreatePhaseHistogram(unsigned channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePhaseHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumData::CreatePowerHistogram(unsigned channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePowerHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumData::CreatePowerDistributionHistogram(unsigned channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePowerDistributionHistogram(name);
    }
#endif


} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATA_HH_ */
