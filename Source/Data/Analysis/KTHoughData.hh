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

#ifdef ROOT_FOUND
#include "TH2.h"
#endif

#include <vector>

namespace Katydid
{

    class KTHoughData : public KTExtensibleData< KTHoughData >
    {
        public:
            KTHoughData();
            virtual ~KTHoughData();

            const KTPhysicalArray< 2, double >* GetTransform(unsigned component = 0) const;
            KTPhysicalArray< 2, double >* GetTransform(unsigned component = 0);

            unsigned GetNComponents() const;

            void SetTransform(KTPhysicalArray< 2, double >* transform, unsigned component = 0);

            KTHoughData& SetNComponents(unsigned nTransforms);

        protected:
            std::vector< KTPhysicalArray< 2, double >* > fTransforms;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateHistogram(unsigned component = 0, const std::string& name = "hHoughSpace") const;
#endif


    };

    inline const KTPhysicalArray< 2, double >* KTHoughData::GetTransform(unsigned component) const
    {
        return fTransforms[component];
    }

    inline KTPhysicalArray< 2, double >* KTHoughData::GetTransform(unsigned component)
    {
        return fTransforms[component];
    }

    inline unsigned KTHoughData::GetNComponents() const
    {
        return unsigned(fTransforms.size());
    }

    inline void KTHoughData::SetTransform(KTPhysicalArray< 2, double >* transform, unsigned component)
    {
        if (component >= fTransforms.size()) SetNComponents(component+1);
        fTransforms[component] = transform;
    }

} /* namespace Katydid */

#endif /* KTHOUGHDATA_HH_ */
