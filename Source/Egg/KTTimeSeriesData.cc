/*
 * KTTimeSeriesData.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTTimeSeriesData.hh"

namespace Katydid
{
    KTTimeSeriesData::KTTimeSeriesData() :
            KTExtensibleData< KTTimeSeriesData >()
    {
    }

    KTTimeSeriesData::~KTTimeSeriesData()
    {
        while (! fComponentData.empty())
        {
            delete fComponentData.back();
            fComponentData.pop_back();
        }
    }

} /* namespace Katydid */
