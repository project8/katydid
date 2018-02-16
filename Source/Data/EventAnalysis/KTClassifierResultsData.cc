/*
 * KTClassifierResultsData.cc
 *
 *  Created on: Feb 16, 2018
 *      Author: ezayas
 */

#include "KTClassifierResultsData.hh"

#include <ostream>

namespace Katydid
{
    const std::string KTClassifierResultsData::sName("classifier-results");

    KTClassifierResultsData::KTClassifierResultsData() :
            KTExtensibleData< KTClassifierResultsData >(),
            fComponent(0)
    {
    }

    KTClassifierResultsData::KTClassifierResultsData(const KTClassifierResultsData& orig) :
            KTExtensibleData< KTClassifierResultsData >(orig),
            
            fComponent(orig.fComponent)
    {
    }

    KTClassifierResultsData::~KTClassifierResultsData()
    {
    }

    KTClassifierResultsData& KTClassifierResultsData::operator=(const KTClassifierResultsData& rhs)
    {
        KTExtensibleData< KTClassifierResultsData >::operator=(rhs);
        fComponent = rhs.fComponent;
        return *this;
    }

} /* namespace Katydid */
