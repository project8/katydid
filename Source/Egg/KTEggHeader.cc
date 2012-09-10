/*
 * KTEggHeader.cc
 *
 *  Created on: Aug 6, 2012
 *      Author: nsoblath
 */

#include "KTEggHeader.hh"

#include "MonarchTypes.hpp"

namespace Katydid
{

    KTEggHeader::KTEggHeader() :
            fFilename(),
            fAcquisitionMode(sOneChannel),
            fRecordSize(0),
            fAcquisitionTime(0),
            fAcquisitionRate(0)
    {
    }

    KTEggHeader::~KTEggHeader()
    {
    }


} /* namespace Katydid */
