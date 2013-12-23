/*
 * KTRawTimeSeriesData.cc
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#include "KTRawTimeSeriesData.hh"

namespace Katydid
{

    KTRawTimeSeriesData::KTRawTimeSeriesData() :
            KTExtensibleData< KTRawTimeSeriesData >()
    {
    }

    KTRawTimeSeriesData::~KTRawTimeSeriesData()
    {
    }

    KTTimeSeriesData& KTTimeSeriesData::SetNComponents(unsigned num)
    {
        unsigned oldSize = fTimeSeries.size();
        // if num < oldSize
        for (unsigned iComponent = num; iComponent < oldSize; iComponent++)
        {
            delete fTimeSeries[iComponent];
        }
        fTimeSeries.resize(num);
        // if num > oldSize
        for (unsigned iComponent = oldSize; iComponent < num; iComponent++)
        {
            fTimeSeries[iComponent] = NULL;
        }
        return *this;
    }

} /* namespace Katydid */
