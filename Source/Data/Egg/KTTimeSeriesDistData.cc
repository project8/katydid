/*
 * KTTimeSeriesDistData.cc
 *
 *  Created on: Jan 16, 2014
 *      Author: laroque
 */

#include "KTTimeSeriesDistData.hh"

namespace Katydid
{
    KTTimeSeriesDistDataCore::KTTimeSeriesDistDataCore() :
            fTimeSeriesDist(1)
    {
        fTimeSeriesDist[0] = NULL;
    }

    KTTimeSeriesDistDataCore::~KTTimeSeriesDistDataCore()
    {
        while (! fTimeSeriesDist.empty())
        {
            delete fTimeSeriesDist.back();
            fTimeSeriesDist.pop_back();
        }
    }

    KTTimeSeriesDistData::KTTimeSeriesDistData() :
            KTTimeSeriesDistDataCore(),
            KTExtensibleData< KTTimeSeriesDistData >()
    {
    }

    KTTimeSeriesDistData::~KTTimeSeriesDistData()
    {
    }

    KTTimeSeriesDistData& KTTimeSeriesDistData::SetNComponents(unsigned num)
    {
        unsigned oldSize = fTimeSeriesDist.size();
        // if num < oldSize
        for (unsigned iComponent = num; iComponent < oldSize; iComponent++)
        {
            delete fTimeSeriesDist[iComponent];
        }
        fTimeSeriesDist.resize(num);
        // if num > oldSize
        for (unsigned iComponent = oldSize; iComponent < num; iComponent++)
        {
            fTimeSeriesDist[iComponent] = NULL;
        }
        return *this;
    }

} /* namespace Katydid */
