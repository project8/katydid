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
#include "TSpline.h"

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
                TSpline* fSpline;
                GainVariation* fGainVar;
            };

        public:
            KTGainVariationData(unsigned nChannels=1);
            virtual ~KTGainVariationData();

            const GainVariation* GetGainVariation(unsigned channelNum = 0) const;
            GainVariation* GetGainVariation(unsigned channelNum = 0);
            const TSpline* GetSpline(unsigned channelNum = 0) const;
            TSpline* GetSpline(unsigned channelNum = 0);
            unsigned GetNChannels() const;

            void SetGainVariation(GainVariation* record, unsigned channelNum = 0);
            void SetSpline(TSpline* spline, unsigned channelNum = 0);
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

    inline const TSpline* KTGainVariationData::GetSpline(unsigned channelNum) const
    {
        return fChannelData[channelNum].fSpline;
    }

    inline TSpline* KTGainVariationData::GetSpline(unsigned channelNum)
    {
        return fChannelData[channelNum].fSpline;
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

    inline void KTGainVariationData::SetSpline(TSpline* spline, unsigned channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fSpline = spline;
    }

    inline void KTGainVariationData::SetNChannels(unsigned channels)
    {
        fChannelData.resize(channels);
        return;
    }


} /* namespace Katydid */

#endif /* KTGAINVARIATIONDATA_HH_ */
