/*
 * KTDiscriminatedPoints1DData.cc
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#include "KTDiscriminatedPoints1DData.hh"

namespace Katydid
{
    KTDiscriminatedPoints1DData::KTDiscriminatedPoints1DData() :
            KTData(),
            fComponentData(1),
            fNBins(1),
            fBinWidth(1.)
    {
    }

    KTDiscriminatedPoints1DData::~KTDiscriminatedPoints1DData()
    {
    }

} /* namespace Katydid */

