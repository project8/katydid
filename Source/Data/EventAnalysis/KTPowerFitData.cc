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
            fCurvature(0.),
            fWidth(0.)
    {
    }

    KTPowerFitData::KTPowerFitData(const KTPowerFitData& orig) :
            KTExtensibleData< KTPowerFitData >(orig),
            
            fCurvature(orig.fCurvature),
            fWidth(orig.fWidth)
    {
    }

    KTPowerFitData::~KTPowerFitData()
    {
    }

    KTPowerFitData& KTPowerFitData::operator=(const KTPowerFitData& rhs)
    {
        KTExtensibleData< KTPowerFitData >::operator=(rhs);
        fCurvature = rhs.fCurvature;
        fWidth = rhs.fWidth;
        return *this;
    }

} /* namespace Katydid */
