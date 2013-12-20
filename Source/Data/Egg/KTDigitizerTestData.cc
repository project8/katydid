/*
 * KTDigitizerTestData.cc
 *
 *  Created on: Dec 18, 2013
 *      Author: nsoblath
 */

#include "KTDigitizerTestData.hh"

namespace Katydid
{

    KTDigitizerTestData::KTDigitizerTestData() :
            KTExtensibleData< KTDigitizerTestData >(),
            fClippingData()
    {
    }

    KTDigitizerTestData::~KTDigitizerTestData()
    {
    }

    unsigned KTDigitizerTestData::GetNComponents() const
    {
        return fClippingData.size();
    }

    KTDigitizerTestData& KTDigitizerTestData::SetNComponents(unsigned num)
    {
        fClippingData.resize(num);
        return *this;
    }

    void KTDigitizerTestData::SetClippingData(unsigned nClipTop, unsigned nClipBottom, double topClipFrac, double bottomClipFrac, unsigned component)
    {
        if (component >= fClippingData.size())
            SetNComponents(component + 1);
        fClippingData[component].fNClipTop = nClipTop;
        fClippingData[component].fNClipBottom = nClipBottom;
        fClippingData[component].fTopClipFrac = topClipFrac;
        fClippingData[component].fBottomClipFrac = bottomClipFrac;
        return;
    }



} /* namespace Katydid */
