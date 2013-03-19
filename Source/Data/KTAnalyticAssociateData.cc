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

    KTAnalyticAssociateData& KTAnalyticAssociateData::SetNComponents(UInt_t num)
    {
        UInt_t oldSize = fTimeSeries.size();
        fTimeSeries.resize(num);
        if (num > oldSize)
        {
            for (UInt_t iComponent = oldSize; iComponent < num; iComponent++)
            {
                fTimeSeries[iComponent] = NULL;
            }
        }
        return *this;
    }


} /* namespace Katydid */
