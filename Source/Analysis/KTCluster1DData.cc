/*
 * KTCluster1DData.cc
 *
 *  Created on: Dec 17, 2012
 *      Author: nsoblath
 */

#include "KTCluster1DData.hh"

namespace Katydid
{
    KTCluster1DData::KTCluster1DData(UInt_t nChannels) :
            KTData(),
            fChannelData(nChannels)
    {
    }

    KTCluster1DData::~KTCluster1DData()
    {
    }

} /* namespace Katydid */

