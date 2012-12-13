/*
 * KTDiscriminatedPoints1DData.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTDiscriminatedPoints1DData.hh"

namespace Katydid
{
    KTDiscriminatedPoints1DData::KTDiscriminatedPoints1DData(UInt_t nChannels) :
            KTData(),
            fChannelData(nChannels)
    {
    }

    KTDiscriminatedPoints1DData::~KTDiscriminatedPoints1DData()
    {
    }

} /* namespace Katydid */

