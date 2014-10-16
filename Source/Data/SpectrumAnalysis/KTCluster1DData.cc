/*
 * KTCluster1DData.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTCluster1DData.hh"

namespace Katydid
{
    const std::string KTCluster1DData::sName("cluster-1d");

    KTCluster1DData::KTCluster1DData() :
            KTExtensibleData< KTCluster1DData >(),
            fComponentData(),
            fNBins(1),
            fBinWidth(1.)
    {
    }

    KTCluster1DData::~KTCluster1DData()
    {
    }

} /* namespace Katydid */

