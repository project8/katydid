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

    class KTHoughData : public KTData< KTHoughData >
    {
        public:
            KTHoughData();
            virtual ~KTHoughData();

            const KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* GetTransform(UInt_t transformNum = 0) const;
            KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* GetTransform(UInt_t transformNum = 0);

            UInt_t GetNTransforms() const;

            void SetTransform(KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* transform, UInt_t transformNum = 0);

            KTHoughData& SetNTransforms(UInt_t nTransforms);

        protected:
            std::vector< KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* > fTransforms;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateHistogram(UInt_t transformNum = 0, const std::string& name = "hHoughSpace") const;
#endif


    };

    inline const KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* KTHoughData::GetTransform(UInt_t component) const
    {
        return fTransforms[component];
    }

    inline KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* KTHoughData::GetTransform(UInt_t component)
    {
        return fTransforms[component];
    }

    inline UInt_t KTHoughData::GetNTransforms() const
    {
        return UInt_t(fTransforms.size());
    }

    inline void KTHoughData::SetTransform(KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* transform, UInt_t transformNum)
    {
        if (transformNum >= fTransforms.size()) fTransforms.resize(transformNum+1);
        fTransforms[transformNum] = transform;
    }

    inline KTHoughData& KTHoughData::SetNTransforms(UInt_t channels)
    {
        fTransforms.resize(channels);
        return *this;
    }


} /* namespace Katydid */

#endif /* KTHOUGHDATA_HH_ */
