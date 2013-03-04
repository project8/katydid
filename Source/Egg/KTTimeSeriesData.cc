/*
 * KTTimeSeriesData.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTTimeSeriesData.hh"

namespace Katydid
{
    KTTimeSeriesDataCore::KTTimeSeriesDataCore() :
            fTimeSeries(1)
    {
        fTimeSeries[0] = NULL;
    }

    KTTimeSeriesDataCore::~KTTimeSeriesDataCore()
    {
        while (! fTimeSeries.empty())
        {
            delete fTimeSeries.back();
            fTimeSeries.pop_back();
        }
    }

    KTTimeSeriesData::KTTimeSeriesData() :
            KTTimeSeriesDataCore(),
            KTExtensibleData< KTTimeSeriesData >()
    {
    }

    KTTimeSeriesData::~KTTimeSeriesData()
    {
    }

} /* namespace Katydid */
