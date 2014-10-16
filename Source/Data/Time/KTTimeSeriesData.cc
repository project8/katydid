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


    const std::string KTTimeSeriesData::sName("time-series");

    KTTimeSeriesData::KTTimeSeriesData() :
            KTTimeSeriesDataCore(),
            KTExtensibleData< KTTimeSeriesData >()
    {
    }

    KTTimeSeriesData::~KTTimeSeriesData()
    {
    }

    KTTimeSeriesData& KTTimeSeriesData::SetNComponents(unsigned num)
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
