/*
 * KTCluster1DData.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTCluster1DData.hh"

namespace Katydid
{
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

