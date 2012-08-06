/*
 * KTEggHeader.cc
 *
 *  Created on: Aug 6, 2012
 *      Author: nsoblath
 */

#include "KTEggHeader.hh"

#include "Monarch.hpp"
#include "MonarchHeader.hpp"

namespace Katydid
{

    KTEggHeader::KTEggHeader() :
            fFilename(""),
            fAcquisitionMode(Monarch::OneChannel),
            fRecordSize(0),
            fAcquisitionTime(0),
            fAcquisitionRate(0)
    {
    }

    KTEggHeader::~KTEggHeader()
    {
    }

    void KTEggHeader::TakeInformation(MonarchHeader* header)
    {
        SetFilename(header->GetFilename());
        SetAcquisitionMode(header->GetAcqMode());
        SetRecordSize(header->GetRecordSize());
        SetAcquisitionTime(header->GetAcqTime());
        SetAcquisitionRate(header->GetAcqRate());
        return;
    }


} /* namespace Katydid */
