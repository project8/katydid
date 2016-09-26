/*
 * KTHoughData.hh
 *
 *  Created on: Nov. 14, 2012
 *      Author: nsoblath
 */

#ifndef KTHOUGHDATA_HH_
#define KTHOUGHDATA_HH_

#include "KTData.hh"

#include "KTPhysicalArray.hh"

#include <vector>

namespace Katydid
{
    

    class KTHoughData : public Nymph::KTExtensibleData< KTHoughData >
    {
        public:
            KTHoughData();
            virtual ~KTHoughData();

            const KTPhysicalArray< 2, double >* GetTransform(unsigned component = 0) const;
            KTPhysicalArray< 2, double >* GetTransform(unsigned component = 0);

            double GetXOffset(unsigned component = 0) const;
            double GetXScale(unsigned component = 0) const;

            double GetYOffset(unsigned component = 0) const;
            double GetYScale(unsigned component = 0) const;

            unsigned GetNComponents() const;

            void SetTransform(KTPhysicalArray< 2, double >* transform, double xOffset = 0., double xScale = 1., double yOffset = 0., double yScale = 1., unsigned component = 0);

            KTHoughData& SetNComponents(unsigned nTransforms);

        private:
            struct PerComponentData
            {
                KTPhysicalArray< 2, double >* fTransform;
                double fXOffset, fXScale;
                double fYOffset, fYScale;
            };
            std::vector< PerComponentData > fTransforms;

        public:
            static const std::string sName;
    };

    inline const KTPhysicalArray< 2, double >* KTHoughData::GetTransform(unsigned component) const
    {
        return fTransforms[component].fTransform;
    }

    inline KTPhysicalArray< 2, double >* KTHoughData::GetTransform(unsigned component)
    {
        return fTransforms[component].fTransform;
    }

    inline double KTHoughData::GetXOffset(unsigned component) const
    {
        return fTransforms[component].fXOffset;
    }
    inline double KTHoughData::GetXScale(unsigned component) const
    {
        return fTransforms[component].fXScale;
    }

    inline double KTHoughData::GetYOffset(unsigned component) const
    {
        return fTransforms[component].fYOffset;
    }
    inline double KTHoughData::GetYScale(unsigned component) const
    {
        return fTransforms[component].fYScale;
    }

    inline unsigned KTHoughData::GetNComponents() const
    {
        return unsigned(fTransforms.size());
    }

    inline void KTHoughData::SetTransform(KTPhysicalArray< 2, double >* transform, double xOffset, double xScale, double yOffset, double yScale, unsigned component)
    {
        if (component >= fTransforms.size()) SetNComponents(component+1);
        fTransforms[component].fTransform = transform;
        fTransforms[component].fXOffset = xOffset;
        fTransforms[component].fXScale = xScale;
        fTransforms[component].fYOffset = yOffset;
        fTransforms[component].fYScale = yScale;
    }

} /* namespace Katydid */

#endif /* KTHOUGHDATA_HH_ */
