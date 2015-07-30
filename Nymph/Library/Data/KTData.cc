/*
 * KTData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTData.hh"

namespace Nymph
{
    const std::string KTData::sName("data");

    KTData::KTData() :
            KTExtensibleData< KTData >(),
            fCounter(0),
            fLastData(false),
            fCutStatus()
    {
    }

    KTData::KTData(const KTData& orig) :
            KTExtensibleData< KTData >(orig),
            fCounter(orig.fCounter),
            fLastData(orig.fLastData),
            fCutStatus(orig.fCutStatus)
    {}

    KTData::~KTData()
    {}

} /* namespace Nymph */
