/*
 * KTGainVariationData.hh
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#ifndef KTGAINVARIATIONDATA_HH_
#define KTGAINVARIATIONDATA_HH_

#include "KTData.hh"

#include "KTGainVariationProcessor.hh"
#include "KTSpline.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <vector>

namespace Katydid
{

    class KTGainVariationData : public KTData< KTGainVariationData >
    {
        public:
            //typedef KTGainVariationProcessor::GainVariation GainVariation;

        protected:
            struct PerComponentData
            {
                KTSpline* fSpline;
                //GainVariation* fGainVar;
            };

        public:
            KTGainVariationData();
            virtual ~KTGainVariationData();

            //const GainVariation* GetGainVariation(UInt_t component = 0) const;
            //GainVariation* GetGainVariation(UInt_t component = 0);
            const KTSpline* GetSpline(UInt_t component = 0) const;
            KTSpline* GetSpline(UInt_t component = 0);

            UInt_t GetNComponents() const;

            //void SetGainVariation(GainVariation* record, UInt_t component = 0);
            void SetSpline(KTSpline* spline, UInt_t component = 0);

            KTGainVariationData& SetNComponents(UInt_t components);

        protected:
            std::vector< PerComponentData > fComponentData;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateGainVariationHistogram(UInt_t nBins, UInt_t component = 0, const std::string& name = "hGainVariation") const;
#endif
    };
/*
    inline const KTGainVariationData::GainVariation* KTGainVariationData::GetGainVariation(UInt_t component) const
    {
        return fChannelData[component].fGainVar;
    }

    inline KTGainVariationData::GainVariation* KTGainVariationData::GetGainVariation(UInt_t component)
    {
        return fChannelData[component].fGainVar;
    }
*/
    inline const KTSpline* KTGainVariationData::GetSpline(UInt_t component) const
    {
        return fComponentData[component].fSpline;
    }

    inline KTSpline* KTGainVariationData::GetSpline(UInt_t component)
    {
        return fComponentData[component].fSpline;
    }

    inline UInt_t KTGainVariationData::GetNComponents() const
    {
        return UInt_t(fComponentData.size());
    }
/*
    inline void KTGainVariationData::SetGainVariation(GainVariation* record, UInt_t component)
    {
        if (component >= fChannelData.size()) fChannelData.resize(component+1);
        fChannelData[component].fGainVar = record;
    }
*/
    inline void KTGainVariationData::SetSpline(KTSpline* spline, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fSpline = spline;
    }

    inline KTGainVariationData& KTGainVariationData::SetNComponents(UInt_t channels)
    {
        fComponentData.resize(channels);
        return *this;
    }


} /* namespace Katydid */

#endif /* KTGAINVARIATIONDATA_HH_ */
