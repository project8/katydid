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
            fChannelData(nChannels),
            fNBinsX(1),
            fNBinsY(1),
            fBinWidthX(1.),
            fBinWidthY(1.)
    {
    }

    KTDiscriminatedPoints2DData::~KTDiscriminatedPoints2DData()
    {
    }

} /* namespace Katydid */

