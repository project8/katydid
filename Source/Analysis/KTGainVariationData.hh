/*
 * KTGainVariationData.hh
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#ifndef KTGAINVARIATIONDATA_HH_
#define KTGAINVARIATIONDATA_HH_

#include "KTWriteableData.hh"

#include "KTFrequencySpectrum.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <vector>

namespace Katydid
{

    class KTGainVariationData : public KTWriteableData
    {
        public:
            KTGainVariationData(unsigned nChannels=1);
            virtual ~KTGainVariationData();

            const KTFrequencySpectrum* GetSpectrum(unsigned channelNum = 0) const;
            KTFrequencySpectrum* GetSpectrum(unsigned channelNum = 0);
            unsigned GetNChannels() const;

            void SetSpectrum(KTFrequencySpectrum* record, unsigned channelNum = 0);
            void SetNChannels(unsigned channels);

            void Accept(KTWriter* writer) const;

        protected:
            static std::string fDefaultName;

            std::vector< KTFrequencySpectrum* > fSpectra;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateGainVariationHistogram(unsigned channelNum = 0, const std::string& name = "hGainVariation") const;
#endif
    };

    inline const KTFrequencySpectrum* KTGainVariationData::GetSpectrum(unsigned channelNum) const
    {
        return fSpectra[channelNum];
    }

    inline KTFrequencySpectrum* KTGainVariationData::GetSpectrum(unsigned channelNum)
    {
        return fSpectra[channelNum];
    }

    inline unsigned KTGainVariationData::GetNChannels() const
    {
        return unsigned(fSpectra.size());
    }

    inline void KTGainVariationData::SetSpectrum(KTFrequencySpectrum* record, unsigned channelNum)
    {
        if (channelNum >= fSpectra.size()) fSpectra.resize(channelNum+1);
        fSpectra[channelNum] = record;
    }

    inline void KTGainVariationData::SetNChannels(unsigned channels)
    {
        fSpectra.resize(channels);
        return;
    }

#ifdef ROOT_FOUND
    inline TH1D* KTGainVariationData::CreateGainVariationHistogram(unsigned channelNum, const std::string& name) const
    {
        return fSpectra[channelNum]->CreateMagnitudeHistogram(name);
    }
#endif


} /* namespace Katydid */

#endif /* KTGAINVARIATIONDATA_HH_ */
