/*
 * KTCluster1DData.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTCluster1DData.hh"

#include "KTDataMap.hh"
#include "KTTIFactory.hh"

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTCluster1DData > > sC1DDMRegistrar;

    KTCluster1DData::KTCluster1DData(UInt_t nGroups) :
            KTData(),
            fGroupData(nGroups),
            fNBins(1),
            fBinWidth(1.)
    {
    }

    KTCluster1DData::~KTCluster1DData()
    {
    }

} /* namespace Katydid */

