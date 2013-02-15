/*
 * KTSliceHeader.cc
 *
 *  Created on: Feb 14, 2013
 *      Author: nsoblath
 */

#include "KTSliceHeader.hh"

namespace Katydid
{
    KTSliceHeader::KTSliceHeader() :
            KTExtensibleData< KTSliceHeader >(),
            fTimeInRun(0.),
            fSliceNumber(0),
            fSliceSize(1),
            fSliceLength(0.),
            fSampleRate(0.),
            fBinWidth(1.),
            fComponentData(1)
    {
    }

    KTSliceHeader::~KTSliceHeader()
    {
    }

} /* namespace Katydid */
