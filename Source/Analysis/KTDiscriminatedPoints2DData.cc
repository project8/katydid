/*
 * KTDiscriminatedPoints2DData.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTDiscriminatedPoints2DData.hh"

namespace Katydid
{
    KTDiscriminatedPoints2DData::KTDiscriminatedPoints2DData(UInt_t nChannels) :
            KTData(),
            fChannelData(nChannels)
    {
    }

    KTDiscriminatedPoints2DData::~KTDiscriminatedPoints2DData()
    {
    }

} /* namespace Katydid */

