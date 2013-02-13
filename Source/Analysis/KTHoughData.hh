/*
 * KTHoughData.hh
 *
 *  Created on: Nov. 14, 2012
 *      Author: nsoblath
 */

#ifndef KTHOUGHDATA_HH_
#define KTHOUGHDATA_HH_

#include "KTWriteableData.hh"

#include "KTPhysicalArray.hh"

#ifdef ROOT_FOUND
#include "TH2.h"
#endif

#include <vector>

namespace Katydid
{

    class KTHoughData : public KTWriteableData
    {
        public:
            KTHoughData(unsigned nTransforms=1);
            virtual ~KTHoughData();

            const KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* GetTransform(unsigned transformNum = 0) const;
            KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* GetTransform(unsigned transformNum = 0);
            unsigned GetNTransforms() const;

            void SetTransform(KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* transform, unsigned transformNum = 0);
            void SetNTransforms(unsigned nTransforms);

            void Accept(KTWriter* writer) const;

        protected:
            std::vector< KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* > fTransforms;

#ifdef ROOT_FOUND
        public:
            virtual TH2D* CreateHistogram(unsigned transformNum = 0, const std::string& name = "hHoughSpace") const;
#endif


    };

    inline const KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* KTHoughData::GetTransform(unsigned component) const
    {
        return fTransforms[component];
    }

    inline KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* KTHoughData::GetTransform(unsigned component)
    {
        return fTransforms[component];
    }

    inline unsigned KTHoughData::GetNTransforms() const
    {
        return unsigned(fTransforms.size());
    }

    inline void KTHoughData::SetTransform(KTPhysicalArray< 1, KTPhysicalArray< 1, Double_t >* >* transform, unsigned transformNum)
    {
        if (transformNum >= fTransforms.size()) fTransforms.resize(transformNum+1);
        fTransforms[transformNum] = transform;
    }

    inline void KTHoughData::SetNTransforms(unsigned channels)
    {
        fTransforms.resize(channels);
        return;
    }


} /* namespace Katydid */

#endif /* KTHOUGHDATA_HH_ */
