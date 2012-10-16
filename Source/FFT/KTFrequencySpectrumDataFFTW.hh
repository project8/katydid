/*
 * KTFrequencySpectrumDataFFTW.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYSPECTRUMDATAFFTW_HH_
#define KTFREQUENCYSPECTRUMDATAFFTW_HH_

#include "KTWriteableData.hh"

#include "KTFrequencySpectrumFFTW.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <vector>

namespace Katydid
{

    class KTFrequencySpectrumDataFFTW : public KTWriteableData
    {
        public:
            KTFrequencySpectrumDataFFTW(unsigned nChannels=1);
            virtual ~KTFrequencySpectrumDataFFTW();

            const std::string& GetName() const;
            static const std::string& StaticGetName();

            const KTFrequencySpectrumFFTW* GetSpectrum(unsigned channelNum = 0) const;
            KTFrequencySpectrumFFTW* GetSpectrum(unsigned channelNum = 0);
            unsigned GetNChannels() const;

            void SetSpectrum(KTFrequencySpectrumFFTW* record, unsigned channelNum = 0);
            void SetNChannels(unsigned channels);

            void Accept(KTWriter* writer) const;

        protected:
            static std::string fName;

            std::vector< KTFrequencySpectrumFFTW* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateMagnitudeHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH1D* CreatePhaseHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH1D* CreatePowerHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPower") const;

            virtual TH1D* CreatePowerDistributionHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPowerDist") const;
#endif
    };

    inline const std::string& KTFrequencySpectrumDataFFTW::GetName() const
    {
        return fName;
    }

    inline const KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTW::GetSpectrum(unsigned channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTW::GetSpectrum(unsigned channelNum)
    {
        return fSpectra[channelNum];
    }

    inline unsigned KTFrequencySpectrumDataFFTW::GetNChannels() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTFrequencySpectrumDataFFTW::SetSpectrum(KTFrequencySpectrumFFTW* record, unsigned channelNum)
    {
        if (channelNum >= fSpectra.size()) fSpectra.resize(channelNum+1);
        fSpectra[channelNum] = record;
    }

    inline void KTFrequencySpectrumDataFFTW::SetNChannels(unsigned channels)
    {
        fSpectra.resize(channels);
        return;
    }

#ifdef ROOT_FOUND
    inline TH1D* KTFrequencySpectrumDataFFTW::CreateMagnitudeHistogram(unsigned channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreateMagnitudeHistogram(name);
    }
    inline TH1D* KTFrequencySpectrumDataFFTW::CreatePhaseHistogram(unsigned channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePhaseHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataFFTW::CreatePowerHistogram(unsigned channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePowerHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataFFTW::CreatePowerDistributionHistogram(unsigned channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePowerDistributionHistogram(name);
    }
#endif


} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATAFFTW_HH_ */
