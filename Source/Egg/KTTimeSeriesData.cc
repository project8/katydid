/*
 * KTTimeSeriesData.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTTimeSeriesData.hh"

#include "KTDataMap.hh"
#include "KTTIFactory.hh"

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTTimeSeriesData > > sTSDMRegistrar;

    KTTimeSeriesData::KTTimeSeriesData() :
            KTWriteableData()
    {
    }

    KTTimeSeriesData::~KTTimeSeriesData()
    {
    }

} /* namespace Katydid */
