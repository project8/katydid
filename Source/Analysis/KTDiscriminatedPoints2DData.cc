/*
 * KTDiscriminatedPoints2DData.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTDiscriminatedPoints2DData.hh"

#include "KTDataMap.hh"
#include "KTTIFactory.hh"

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTDiscriminatedPoints2DData > > sDP2DDMRegistrar;

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

