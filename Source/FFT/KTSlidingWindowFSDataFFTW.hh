/*
 * KTSlidingWindowFSDataFFTW.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTSLIDINGWINDOWFSDATAFFTW_HH_
#define KTSLIDINGWINDOWFSDATAFFTW_HH_

#include "KTWriteableData.hh"

#include "KTFrequencySpectrumFFTW.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#endif

#include <vector>

namespace Katydid
{

    class KTSlidingWindowFSDataFFTW : public KTWriteableData
    {
        public:
            KTSlidingWindowFSDataFFTW(unsigned nChannels=1);
            virtual ~KTSlidingWindowFSDataFFTW();

            const std::string& GetName() const;
            static const std::string& StaticGetName();

            const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* GetSpectra(unsigned channelNum = 0) const;
            KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* GetSpectra(unsigned channelNum = 0);
            unsigned GetNChannels() const;

            void SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* spectra, unsigned channelNum = 0);
            void SetNChannels(unsigned channels);

            void Accept(KTWriter* writer) const;

        protected:
            static std::string fName;

            std::vector< KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateMagnitudeHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH2D* CreatePhaseHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH2D* CreatePowerHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPower") const;
#endif


    };

    inline const std::string& KTSlidingWindowFSDataFFTW::GetName() const
    {
        return fName;
    }

    inline const KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* KTSlidingWindowFSDataFFTW::GetSpectra(unsigned channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* KTSlidingWindowFSDataFFTW::GetSpectra(unsigned channelNum)
    {
        return fSpectra[channelNum];
    }

    inline unsigned KTSlidingWindowFSDataFFTW::GetNChannels() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTSlidingWindowFSDataFFTW::SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumFFTW* >* spectra, unsigned channelNum)
    {
        if (channelNum >= fSpectra.size()) fSpectra.resize(channelNum+1);
        fSpectra[channelNum] = spectra;
    }

    inline void KTSlidingWindowFSDataFFTW::SetNChannels(unsigned channels)
    {
        fSpectra.resize(channels);
        return;
    }


} /* namespace Katydid */

#endif /* KTSLIDINGWINDOWFSDATAFFTW_HH_ */
