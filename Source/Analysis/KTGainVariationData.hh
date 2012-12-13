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
            //typedef KTGainVariationProcessor::GainVariation GainVariation;

        protected:
            struct PerChannelData
            {
                TSpline* fSpline;
                //GainVariation* fGainVar;
            };

        public:
            KTGainVariationData(UInt_t nChannels=1);
            virtual ~KTGainVariationData();

            //const GainVariation* GetGainVariation(UInt_t channelNum = 0) const;
            //GainVariation* GetGainVariation(UInt_t channelNum = 0);
            const TSpline* GetSpline(UInt_t channelNum = 0) const;
            TSpline* GetSpline(UInt_t channelNum = 0);
            UInt_t GetNChannels() const;

            //void SetGainVariation(GainVariation* record, UInt_t channelNum = 0);
            void SetSpline(TSpline* spline, UInt_t channelNum = 0);
            void SetNChannels(UInt_t channels);

            void Accept(KTWriter* writer) const;

        protected:
            static std::string fDefaultName;

            std::vector< PerChannelData > fChannelData;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateGainVariationHistogram(UInt_t nBins, UInt_t channelNum = 0, const std::string& name = "hGainVariation") const;
#endif
    };
/*
    inline const KTGainVariationData::GainVariation* KTGainVariationData::GetGainVariation(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fGainVar;
    }

    inline KTGainVariationData::GainVariation* KTGainVariationData::GetGainVariation(UInt_t channelNum)
    {
        return fChannelData[channelNum].fGainVar;
    }
*/
    inline const TSpline* KTGainVariationData::GetSpline(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fSpline;
    }

    inline TSpline* KTGainVariationData::GetSpline(UInt_t channelNum)
    {
        return fChannelData[channelNum].fSpline;
    }

    inline UInt_t KTGainVariationData::GetNChannels() const
    {
        return UInt_t(fChannelData.size());
    }
/*
    inline void KTGainVariationData::SetGainVariation(GainVariation* record, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fGainVar = record;
    }
*/
    inline void KTGainVariationData::SetSpline(TSpline* spline, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fSpline = spline;
    }

    inline void KTGainVariationData::SetNChannels(UInt_t channels)
    {
        fChannelData.resize(channels);
        return;
    }


} /* namespace Katydid */

#endif /* KTGAINVARIATIONDATA_HH_ */
