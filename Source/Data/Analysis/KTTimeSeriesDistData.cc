/*
 * KTTimeSeriesDistData.cc
 *
 *  Created on: Jan 22, 2014
 *      Author: laroque
 */

#include "KTTimeSeriesDistData.hh"

#include "KTTimeSeriesDist.hh"

namespace Katydid
{
    const std::string KTTimeSeriesDistData::sName("time-series-dist");

    KTTimeSeriesDistData::KTTimeSeriesDistData() :
            KTExtensibleData< KTTimeSeriesDistData >()
    {
    }

    KTTimeSeriesDistData::~KTTimeSeriesDistData()
    {
        while (! fTimeSeriesDist.empty())
        {
            delete fTimeSeriesDist.back();
            fTimeSeriesDist.pop_back();
        }
    }

    KTTimeSeriesDistData& KTTimeSeriesDistData::SetNComponents(unsigned num)
    {
        unsigned oldSize = fTimeSeriesDist.size();
        // if num < oldSize
        for (unsigned iComponent = num; iComponent < oldSize; ++iComponent)
        {
            delete fTimeSeriesDist[iComponent];
        }
        fTimeSeriesDist.resize(num);
        // if num > oldSize
        for (unsigned iComponent = oldSize; iComponent < num; ++iComponent)
        {
            fTimeSeriesDist[iComponent] = NULL;
        }
        return *this;
    }

} /* namespace Katydid */
