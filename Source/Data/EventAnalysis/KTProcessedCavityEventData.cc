/*
 * KTProcessedCavityEventData.cc
 *
 *  Created on: Sep 11, 2025
 *      Author: juniorpe
 */

#include "KTProcessedCavityEventData.hh"

#include "KTLogger.hh"

namespace Katydid
{

    const std::string KTProcessedCavityEventData::sName("proc-cavity-event");

    KTProcessedCavityEventData::KTProcessedCavityEventData() :
            KTExtensibleData< KTProcessedCavityEventData >(),
            fComponent(0),
            fAcquisitionID(0.),
            fEventID(0),
            fTotalEventSequences(0),
            fFirstTrackStartCyclotronFrequency(0.),
            fFirstTrackAxialFrequency(0.),
            fMPTEventSequenceID(),
            fMPTStartCyclotronFrequency(),
            fMPTAxialFrequency()
            
    {
    }

    KTProcessedCavityEventData::KTProcessedCavityEventData(const KTProcessedCavityEventData& orig) :
            KTExtensibleData< KTProcessedCavityEventData >(orig),
            
            fComponent(orig.fComponent),
            fAcquisitionID(orig.fAcquisitionID),
            fEventID(orig.fEventID),
            fTotalEventSequences(orig.fTotalEventSequences),
            fFirstTrackStartCyclotronFrequency(orig.fFirstTrackStartCyclotronFrequency),
            fFirstTrackAxialFrequency(orig.fFirstTrackAxialFrequency),
            fMPTEventSequenceID(orig.fMPTEventSequenceID),
            fMPTStartCyclotronFrequency(orig.fMPTStartCyclotronFrequency),
            fMPTAxialFrequency(orig.fMPTAxialFrequency)
    {
    }

    KTProcessedCavityEventData::~KTProcessedCavityEventData()
    {
    }

    KTProcessedCavityEventData& KTProcessedCavityEventData::operator=(const KTProcessedCavityEventData& rhs)
    {
        KTExtensibleData< KTProcessedCavityEventData >::operator=(rhs);
        fComponent = rhs.fComponent;
        fAcquisitionID = rhs.fAcquisitionID;
        fEventID = rhs.fEventID;
        fTotalEventSequences = rhs.fTotalEventSequences;
        fFirstTrackStartCyclotronFrequency = rhs.fFirstTrackStartCyclotronFrequency;
        fFirstTrackAxialFrequency = rhs.fFirstTrackAxialFrequency;
        fMPTEventSequenceID = rhs.fMPTEventSequenceID;
        fMPTStartCyclotronFrequency = rhs.fMPTStartCyclotronFrequency;
        fMPTAxialFrequency = rhs.fMPTAxialFrequency;
        return *this;
    }

}
