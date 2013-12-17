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

            const KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* GetTransform(UInt_t component = 0) const;
            KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* GetTransform(UInt_t component = 0);

            UInt_t GetNComponents() const;

            void SetTransform(KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* transform, UInt_t component = 0);

            KTHoughData& SetNComponents(UInt_t nTransforms);

        protected:
            std::vector< KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* > fTransforms;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateHistogram(UInt_t component = 0, const std::string& name = "hHoughSpace") const;
#endif


    };

    inline const KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* KTHoughData::GetTransform(UInt_t component) const
    {
        return fTransforms[component];
    }

    inline KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* KTHoughData::GetTransform(UInt_t component)
    {
        return fTransforms[component];
    }

    inline UInt_t KTHoughData::GetNComponents() const
    {
        return UInt_t(fTransforms.size());
    }

    inline void KTHoughData::SetTransform(KTPhysicalArray< 1, KTPhysicalArray< 1, double >* >* transform, UInt_t component)
    {
        if (component >= fTransforms.size()) SetNComponents(component+1);
        fTransforms[component] = transform;
    }

} /* namespace Katydid */

#endif /* KTHOUGHDATA_HH_ */
