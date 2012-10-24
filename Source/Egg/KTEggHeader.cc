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
            fNChannels(1),
            fRecordSize(0),
            fMonarchRecordSize(0),
            fAcquisitionTime(0),
            fAcquisitionRate(0)
    {
    }

    KTEggHeader::KTEggHeader(const KTEggHeader& orig) :
            fFilename(orig.fFilename),
            fAcquisitionMode(orig.fAcquisitionMode),
            fNChannels(orig.fNChannels),
            fRecordSize(orig.fRecordSize),
            fMonarchRecordSize(orig.fMonarchRecordSize),
            fAcquisitionTime(orig.fAcquisitionTime),
            fAcquisitionRate(orig.fAcquisitionRate)
    {
    }

    KTEggHeader::~KTEggHeader()
    {
    }


} /* namespace Katydid */
