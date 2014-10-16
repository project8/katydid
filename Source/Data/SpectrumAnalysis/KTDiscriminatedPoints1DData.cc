/*
 * KTDiscriminatedPoints1DData.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTDiscriminatedPoints1DData.hh"

namespace Katydid
{
    const std::string KTDiscriminatedPoints1DData::sName("disc-points-1d");

    KTDiscriminatedPoints1DData::KTDiscriminatedPoints1DData() :
            KTExtensibleData< KTDiscriminatedPoints1DData >(),
            fComponentData(1),
            fNBins(1),
            fBinWidth(1.)
    {
    }

    KTDiscriminatedPoints1DData::~KTDiscriminatedPoints1DData()
    {
    }

} /* namespace Katydid */

