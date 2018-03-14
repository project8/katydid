/*
 * KTProcessedMPTData.cc
 *
 *  Created on: Mar 14, 2018
 *      Author: ezayas
 */

#include "KTProcessedMPTData.hh"

#include "KTLogger.hh"

namespace Katydid
{

    const std::string KTProcessedMPTData::sName("proc-mpt");

    KTProcessedMPTData::KTProcessedMPTData() :
            KTExtensibleData< KTProcessedMPTData >(),
            fComponent(0),
            fMainTrack(),
            fAxialFrequency(0.)
    {
    }

    KTProcessedMPTData::KTProcessedMPTData(const KTProcessedMPTData& orig) :
            KTExtensibleData< KTProcessedMPTData >(orig),
            
            fComponent(orig.fComponent),
            fMainTrack(orig.fMainTrack),
            fAxialFrequency(orig.fAxialFrequency)
    {
    }

    KTProcessedMPTData::~KTProcessedMPTData()
    {
    }

    KTProcessedMPTData& KTProcessedMPTData::operator=(const KTProcessedMPTData& rhs)
    {
        KTExtensibleData< KTProcessedMPTData >::operator=(rhs);
        fComponent = rhs.fComponent;
        fMainTrack = rhs.fMainTrack;
        fAxialFrequency = rhs.fAxialFrequency;
        return *this;
    }

}
