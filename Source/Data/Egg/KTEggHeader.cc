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
            fSliceSize(0),
            fRecordSize(0),
            fAcquisitionTime(0),
            fAcquisitionRate(0)
    {
    }

    KTEggHeader::KTEggHeader(const KTEggHeader& orig) :
            fFilename(orig.fFilename),
            fAcquisitionMode(orig.fAcquisitionMode),
            fNChannels(orig.fNChannels),
            fSliceSize(orig.fSliceSize),
            fRecordSize(orig.fRecordSize),
            fAcquisitionTime(orig.fAcquisitionTime),
            fAcquisitionRate(orig.fAcquisitionRate)
    {
    }

    KTEggHeader::~KTEggHeader()
    {
    }

    KTEggHeader& KTEggHeader::operator=(const KTEggHeader& rhs)
    {
        fFilename = rhs.fFilename;
        fAcquisitionMode = rhs.fAcquisitionMode;
        fNChannels = rhs.fNChannels;
        fSliceSize = rhs.fSliceSize;
        fRecordSize = rhs.fRecordSize;
        fAcquisitionTime = rhs.fAcquisitionTime;
        fAcquisitionRate = rhs.fAcquisitionRate;
        return *this;
    }

} /* namespace Katydid */
