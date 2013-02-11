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
            KTFrequencySpectrumDataFFTW(UInt_t nChannels=1);
            virtual ~KTFrequencySpectrumDataFFTW();

            const KTFrequencySpectrumFFTW* GetSpectrumFFTW(UInt_t channelNum = 0) const;
            KTFrequencySpectrumFFTW* GetSpectrumFFTW(UInt_t channelNum = 0);

            const KTFrequencySpectrumFFTW* GetSpectrum(UInt_t channelNum = 0) const;
            KTFrequencySpectrumFFTW* GetSpectrum(UInt_t channelNum = 0);
            unsigned GetNChannels() const;
            Double_t GetTimeInRun() const;
            ULong64_t GetSliceNumber() const;

            void SetSpectrum(KTFrequencySpectrumFFTW* record, unsigned channelNum = 0);

            void SetNChannels(UInt_t channels);
            void SetTimeInRun(Double_t tir);
            void SetSliceNumber(ULong64_t slice);

            void Accept(KTWriter* writer) const;

        protected:
            std::vector< KTFrequencySpectrumFFTW* > fSpectra;

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

    inline const KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTW::GetSpectrumFFTW(UInt_t channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTW::GetSpectrumFFTW(UInt_t channelNum)
    {
        return fSpectra[channelNum];
    }

    inline const KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTW::GetSpectrum(UInt_t channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTFrequencySpectrumFFTW* KTFrequencySpectrumDataFFTW::GetSpectrum(UInt_t channelNum)
    {
        return fSpectra[channelNum];
    }

    inline UInt_t KTFrequencySpectrumDataFFTW::GetNChannels() const
    {
        return UInt_t(fSpectra.size());
    }

    inline Double_t KTFrequencySpectrumDataFFTW::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline ULong64_t KTFrequencySpectrumDataFFTW::GetSliceNumber() const
    {
        return fSliceNumber;
    }

    inline void KTFrequencySpectrumDataFFTW::SetSpectrum(KTFrequencySpectrumFFTW* record, UInt_t channelNum)
    {
        if (channelNum >= fSpectra.size()) fSpectra.resize(channelNum+1);
        fSpectra[channelNum] = record;
    }

    inline void KTFrequencySpectrumDataFFTW::SetNChannels(UInt_t channels)
    {
        fSpectra.resize(channels);
        return;
    }

    inline void KTFrequencySpectrumDataFFTW::SetTimeInRun(Double_t tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTFrequencySpectrumDataFFTW::SetSliceNumber(ULong64_t slice)
    {
        fSliceNumber = slice;
        return;
    }

#ifdef ROOT_FOUND
    inline TH1D* KTFrequencySpectrumDataFFTW::CreateMagnitudeHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreateMagnitudeHistogram(name);
    }
    inline TH1D* KTFrequencySpectrumDataFFTW::CreatePhaseHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePhaseHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataFFTW::CreatePowerHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePowerHistogram(name);
    }

    inline TH1D* KTFrequencySpectrumDataFFTW::CreatePowerDistributionHistogram(UInt_t channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreatePowerDistributionHistogram(name);
    }
#endif


} /* namespace Katydid */

#endif /* KTFREQUENCYSPECTRUMDATAFFTW_HH_ */
