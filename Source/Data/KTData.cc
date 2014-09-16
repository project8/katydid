/*
 * KTData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTData.hh"

namespace Katydid
{
    const std::string KTData::sName("data");

    KTData::KTData() :
            fCounter(0),
            fLastData(false)
    {
    }

    KTData::~KTData()
    {}

} /* namespace Katydid */
