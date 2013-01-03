/*
 * KTTimeSeriesData.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTTimeSeriesData.hh"

#include "KTTIFactory.hh"

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTTimeSeriesData > > sTSRegistrar;

    KTTimeSeriesData::KTTimeSeriesData() :
            KTWriteableData()
    {
    }

    KTTimeSeriesData::~KTTimeSeriesData()
    {
    }

} /* namespace Katydid */
