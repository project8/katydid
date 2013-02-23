/*
 * KTSlidingWindowFSData.hh
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#ifndef KTSLIDINGWINDOWFSDATA_HH_
#define KTSLIDINGWINDOWFSDATA_HH_

#include "KTWriteableData.hh"

#include "KTFrequencySpectrumPolar.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#endif

#include <vector>

namespace Katydid
{

    class KTSlidingWindowFSData : public KTWriteableData
    {
        public:
            KTSlidingWindowFSData(unsigned nChannels=1);
            virtual ~KTSlidingWindowFSData();

            const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* GetSpectra(unsigned channelNum = 0) const;
            KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* GetSpectra(unsigned channelNum = 0);
            unsigned GetNChannels() const;

            void SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* spectra, unsigned channelNum = 0);
            void SetNChannels(unsigned channels);

            void Accept(KTWriter* writer) const;

        protected:
            std::vector< KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateMagnitudeHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumMag") const;
            virtual TH2D* CreatePhaseHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPhase") const;

            virtual TH2D* CreatePowerHistogram(unsigned channelNum = 0, const std::string& name = "hFrequencySpectrumPower") const;
#endif


    };

    inline const KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* KTSlidingWindowFSData::GetSpectra(unsigned channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* KTSlidingWindowFSData::GetSpectra(unsigned channelNum)
    {
        return fSpectra[channelNum];
    }

    inline unsigned KTSlidingWindowFSData::GetNChannels() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTSlidingWindowFSData::SetSpectra(KTPhysicalArray< 1, KTFrequencySpectrumPolar* >* spectra, unsigned channelNum)
    {
        if (channelNum >= fSpectra.size()) fSpectra.resize(channelNum+1);
        fSpectra[channelNum] = spectra;
    }

    inline void KTSlidingWindowFSData::SetNChannels(unsigned channels)
    {
        fSpectra.resize(channels);
        return;
    }

} /* namespace Katydid */

#endif /* KTSLIDINGWINDOWFSDATA_HH_ */
