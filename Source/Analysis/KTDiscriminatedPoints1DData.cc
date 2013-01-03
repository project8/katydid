/*
 * KTDiscriminatedPoints1DData.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTDiscriminatedPoints1DData.hh"

#include "KTDataMap.hh"
#include "KTTIFactory.hh"

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTDiscriminatedPoints1DData > > sDP1DDMRegistrar;

    KTDiscriminatedPoints1DData::KTDiscriminatedPoints1DData(UInt_t nChannels) :
            KTData(),
            fChannelData(nChannels)
    {
    }

    KTDiscriminatedPoints1DData::~KTDiscriminatedPoints1DData()
    {
    }

} /* namespace Katydid */

