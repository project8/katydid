/*
 * KTGainVariationData.hh
 *
 *  Created on: Dec 10, 2012
 *      Author: nsoblath
 */

#ifndef KTGAINVARIATIONDATA_HH_
#define KTGAINVARIATIONDATA_HH_

#include "KTData.hh"

#include "KTSpline.hh"

#ifdef ROOT_FOUND
#include "TH1.h"
#endif

#include <vector>

namespace Katydid
{
    

    class KTGainVariationData : public Nymph::KTExtensibleData< KTGainVariationData >
    {
        protected:
            struct PerComponentData
            {
                KTSpline* fSpline;
                KTSpline* fVarianceSpline;
            };

        public:
            KTGainVariationData();
            KTGainVariationData(const KTGainVariationData& orig);
            virtual ~KTGainVariationData();

            KTGainVariationData& operator=(const KTGainVariationData& rhs);

            const KTSpline* GetSpline(unsigned component = 0) const;
            KTSpline* GetSpline(unsigned component = 0);

            const KTSpline* GetVarianceSpline(unsigned component = 0) const;
            KTSpline* GetVarianceSpline(unsigned component = 0);

            unsigned GetNComponents() const;

            void SetSpline(KTSpline* spline, unsigned component = 0);
            void SetVarianceSpline(KTSpline* spline, unsigned component = 0);

            KTGainVariationData& SetNComponents(unsigned components);

        private:
            std::vector< PerComponentData > fComponentData;

        public:
            static const std::string sName;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateGainVariationHistogram(unsigned nBins, unsigned component = 0, const std::string& name = "hGainVariation") const;
            virtual TH1D* CreateGainVariationVarianceHistogram(unsigned nBins, unsigned component = 0, const std::string& name = "hGainVariationVariance") const;
#endif
    };

    inline const KTSpline* KTGainVariationData::GetSpline(unsigned component) const
    {
        return fComponentData[component].fSpline;
    }

    inline KTSpline* KTGainVariationData::GetSpline(unsigned component)
    {
        return fComponentData[component].fSpline;
    }

    inline const KTSpline* KTGainVariationData::GetVarianceSpline(unsigned component) const
    {
        return fComponentData[component].fVarianceSpline;
    }

    inline KTSpline* KTGainVariationData::GetVarianceSpline(unsigned component)
    {
        return fComponentData[component].fVarianceSpline;
    }

    inline unsigned KTGainVariationData::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline void KTGainVariationData::SetSpline(KTSpline* spline, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        delete fComponentData[component].fSpline;
        fComponentData[component].fSpline = spline;
    }

    inline void KTGainVariationData::SetVarianceSpline(KTSpline* spline, unsigned component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        delete fComponentData[component].fVarianceSpline;
        fComponentData[component].fVarianceSpline = spline;
    }

} /* namespace Katydid */

#endif /* KTGAINVARIATIONDATA_HH_ */
