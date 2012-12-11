/*
 * KTGainVariationData.hh
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#ifndef KTGAINVARIATIONDATA_HH_
#define KTGAINVARIATIONDATA_HH_

#include "KTWriteableData.hh"

#include "KTGainVariationProcessor.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <vector>

namespace Katydid
{

    class KTGainVariationData : public KTWriteableData
    {
        public:
            typedef KTGainVariationProcessor::GainVariation GainVariation;

        protected:
            struct PerChannelData
            {
                KTGainVariationProcessor::FitResult fFitResult;
                GainVariation* fGainVar;
            };

        public:
            KTGainVariationData(unsigned nChannels=1);
            virtual ~KTGainVariationData();

            const GainVariation* GetGainVariation(unsigned channelNum = 0) const;
            GainVariation* GetGainVariation(unsigned channelNum = 0);
            const KTGainVariationProcessor::FitResult& GetFitResult(unsigned channelNum = 0) const;
            unsigned GetNChannels() const;

            void SetGainVariation(GainVariation* record, unsigned channelNum = 0);
            void SetFitResults(const KTGainVariationProcessor::FitResult& results, unsigned channelNum = 0);
            void SetNChannels(unsigned channels);

            void Accept(KTWriter* writer) const;

        protected:
            static std::string fDefaultName;

            std::vector< PerChannelData > fChannelData;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateGainVariationHistogram(unsigned channelNum = 0, const std::string& name = "hGainVariation") const;
#endif
    };

    inline const KTGainVariationData::GainVariation* KTGainVariationData::GetGainVariation(unsigned channelNum) const
    {
        return fChannelData[channelNum].fGainVar;
    }

    inline KTGainVariationData::GainVariation* KTGainVariationData::GetGainVariation(unsigned channelNum)
    {
        return fChannelData[channelNum].fGainVar;
    }

    inline const KTGainVariationProcessor::FitResult& KTGainVariationData::GetFitResult(unsigned channelNum) const
    {
        return fChannelData[channelNum].fFitResult;
    }

    inline unsigned KTGainVariationData::GetNChannels() const
    {
        return unsigned(fChannelData.size());
    }

    inline void KTGainVariationData::SetGainVariation(GainVariation* record, unsigned channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fGainVar = record;
    }

    inline void KTGainVariationData::SetFitResults(const KTGainVariationProcessor::FitResult& results, unsigned channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fFitResult = results;
    }

    inline void KTGainVariationData::SetNChannels(unsigned channels)
    {
        fChannelData.resize(channels);
        return;
    }


} /* namespace Katydid */

#endif /* KTGAINVARIATIONDATA_HH_ */
