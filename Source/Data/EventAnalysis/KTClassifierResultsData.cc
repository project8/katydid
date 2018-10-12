/*
 * KTClassifierResultsData.cc
 *
 *  Created on: Feb 16, 2018
 *      Author: ezayas, L. Saldana
 */

#include "KTClassifierResultsData.hh"

namespace Katydid
{
    const std::string KTClassifierResultsData::sName("classifier-results");

    KTClassifierResultsData::KTClassifierResultsData() :
            KTExtensibleData< KTClassifierResultsData >(),
            fComponent(0),
            fMainCarrierHigh(0),
            fMainCarrierLow(0),
            fSideBand(0)    
    {
    }

    KTClassifierResultsData::KTClassifierResultsData(const KTClassifierResultsData& orig) :
            KTExtensibleData< KTClassifierResultsData >(orig),
            fComponent(orig.fComponent),         
            fMainCarrierHigh(orig.fMainCarrierHigh),
            fMainCarrierLow(orig.fMainCarrierLow),
            fSideBand(orig.fSideBand)
    {
    }

    KTClassifierResultsData::~KTClassifierResultsData()
    {
    }

    KTClassifierResultsData& KTClassifierResultsData::operator=(const KTClassifierResultsData& rhs)
    {
        KTExtensibleData< KTClassifierResultsData >::operator=(rhs);
        fComponent = rhs.fComponent;
        fMainCarrierHigh = rhs.fMainCarrierHigh;
        fMainCarrierLow = rhs.fMainCarrierLow;
        fSideBand = rhs.fSideBand;
        return *this;
    }

} /* namespace Katydid */
