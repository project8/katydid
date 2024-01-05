/*
 * KTRawTimeSeriesData.cc
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#include "KTRawTimeSeriesData.hh"

#include "KTRawTimeSeries.hh"

namespace Katydid
{
    KTRawTimeSeriesData::KTRawTimeSeriesData() :
            Nymph::Data()
    {
    }

    KTRawTimeSeriesData::~KTRawTimeSeriesData()
    {
        while (! fTimeSeries.empty())
        {
            delete fTimeSeries.back();
            fTimeSeries.pop_back();
        }
    }

    KTRawTimeSeriesData& KTRawTimeSeriesData::SetNComponents(unsigned num)
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
