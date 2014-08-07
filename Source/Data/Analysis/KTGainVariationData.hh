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

    class KTGainVariationData : public KTExtensibleData< KTGainVariationData >
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
            KTGainVariationData(const KTGainVariationData& orig);
            virtual ~KTGainVariationData();

            KTGainVariationData& operator=(const KTGainVariationData& rhs);

            //const GainVariation* GetGainVariation(unsigned component = 0) const;
            //GainVariation* GetGainVariation(unsigned component = 0);
            const KTSpline* GetSpline(unsigned component = 0) const;
            KTSpline* GetSpline(unsigned component = 0);

            unsigned GetNComponents() const;

            //void SetGainVariation(GainVariation* record, unsigned component = 0);
            void SetSpline(KTSpline* spline, unsigned component = 0);

            KTGainVariationData& SetNComponents(unsigned components);

        protected:
            std::vector< PerComponentData > fComponentData;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateGainVariationHistogram(unsigned nBins, unsigned component = 0, const std::string& name = "hGainVariation") const;
#endif
    };
/*
    inline const KTGainVariationData::GainVariation* KTGainVariationData::GetGainVariation(unsigned component) const
    {
        return fChannelData[component].fGainVar;
    }

    inline KTGainVariationData::GainVariation* KTGainVariationData::GetGainVariation(unsigned component)
    {
        return fChannelData[component].fGainVar;
    }
*/
    inline const KTSpline* KTGainVariationData::GetSpline(unsigned component) const
    {
        return fComponentData[component].fSpline;
    }

    inline KTSpline* KTGainVariationData::GetSpline(unsigned component)
    {
        return fComponentData[component].fSpline;
    }

    inline unsigned KTGainVariationData::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }
/*
    inline void KTGainVariationData::SetGainVariation(GainVariation* record, unsigned component)
    {
        if (component >= fChannelData.size()) fChannelData.resize(component+1);
        fChannelData[component].fGainVar = record;
    }
*/
    inline void KTGainVariationData::SetSpline(KTSpline* spline, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        delete fComponentData[component].fSpline;
        fComponentData[component].fSpline = spline;
    }

} /* namespace Katydid */

#endif /* KTGAINVARIATIONDATA_HH_ */
