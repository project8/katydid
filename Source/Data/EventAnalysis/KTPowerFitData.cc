/*
 * KTPowerFitData.cc
 *
 *  Created on: Oct 27, 2016
 *      Author: ezayas
 */

#include "KTPowerFitData.hh"

#include <ostream>

namespace Katydid
{
    const std::string KTPowerFitData::sName("power-fit");

    KTPowerFitData::KTPowerFitData() :
            KTExtensibleData< KTPowerFitData >(),
            fComponentData(1.)
    {
    }

    KTPowerFitData::KTPowerFitData(const KTPowerFitData& orig) :
            KTExtensibleData< KTPowerFitData >(orig),
            
            fComponentData(orig.fComponentData)
    {
    }

    KTPowerFitData::~KTPowerFitData()
    {
    }

    KTPowerFitData& KTPowerFitData::operator=(const KTPowerFitData& rhs)
    {
        KTExtensibleData< KTPowerFitData >::operator=(rhs);
        fComponentData = rhs.fComponentData;
        return *this;
    }

} /* namespace Katydid */
