/*
 * KTDigitizerTestData.cc
 *
 *  Created on: Dec 18, 2013
 *      Author: nsoblath
 */

#include "KTDigitizerTestData.hh"

namespace Katydid
{

    KTDigitizerTestData::KTDigitizerTestData() :
            KTExtensibleData< KTDigitizerTestData >()
    {
    }

    KTDigitizerTestData::~KTDigitizerTestData()
    {
    }

    unsigned KTDigitizerTestData::GetNComponents() const
    {
        return 0;
    }

    KTDigitizerTestData& KTDigitizerTestData::SetNComponents(unsigned num)
    {
        return *this;
    }


} /* namespace Katydid */
