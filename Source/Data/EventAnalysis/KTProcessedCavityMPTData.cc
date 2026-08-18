/*
 * KTProcessedCavityMPTData.cc
 *
 *  Created on: Aug 19, 2025
 *      Author: juniorpe
 */

#include "KTProcessedCavityMPTData.hh"

#include "KTLogger.hh"

namespace Katydid
{

    const std::string KTProcessedCavityMPTData::sName("proc-cavity-mpt");

    KTProcessedCavityMPTData::KTProcessedCavityMPTData() :
            KTExtensibleData< KTProcessedCavityMPTData >(),
            fComponent(0),
            fAxialFrequency(0.)
    {
    }

    KTProcessedCavityMPTData::KTProcessedCavityMPTData(const KTProcessedCavityMPTData& orig) :
            KTExtensibleData< KTProcessedCavityMPTData >(orig),
            
            fComponent(orig.fComponent),
            fAxialFrequency(orig.fAxialFrequency)
    {
    }

    KTProcessedCavityMPTData::~KTProcessedCavityMPTData()
    {
    }

    KTProcessedCavityMPTData& KTProcessedCavityMPTData::operator=(const KTProcessedCavityMPTData& rhs)
    {
        KTExtensibleData< KTProcessedCavityMPTData >::operator=(rhs);
        fComponent = rhs.fComponent;
        fAxialFrequency = rhs.fAxialFrequency;
        return *this;
    }

}
