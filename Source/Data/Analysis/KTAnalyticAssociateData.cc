/*
 * KTAnalyticAssociator.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTAnalyticAssociateData.hh"

namespace Katydid
{
    KTAnalyticAssociateData::KTAnalyticAssociateData() :
        KTTimeSeriesDataCore(),
        KTExtensibleData< KTAnalyticAssociateData >()
    {}

    KTAnalyticAssociateData::~KTAnalyticAssociateData()
    {}

    KTAnalyticAssociateData& KTAnalyticAssociateData::SetNComponents(unsigned num)
    {
        unsigned oldSize = fTimeSeries.size();
        // if num < oldSize
        for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
        {
            delete fTimeSeries[iComponent];
        }
        fTimeSeries.resize(num);
        // if num > oldSize
        for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
        {
            fTimeSeries[iComponent] = NULL;
        }
        return *this;
    }


} /* namespace Katydid */
