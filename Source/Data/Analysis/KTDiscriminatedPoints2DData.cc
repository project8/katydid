/*
 * KTDiscriminatedPoints2DData.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTDiscriminatedPoints2DData.hh"

namespace Katydid
{
    const std::string KTDiscriminatedPoints2DData::sName("disc-points-2d");

    KTDiscriminatedPoints2DData::KTDiscriminatedPoints2DData() :
            KTExtensibleData< KTDiscriminatedPoints2DData >(),
            fComponentData(1),
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

