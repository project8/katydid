/*
 * KTCavityEventProcessing.cc
 *
 *  Created on: Sep 11, 2025
 *      Author: juniorpe
 */

#include "KTCavityEventProcessing.hh"

#include "KTLogger.hh"
#include "KTProcessedTrackData.hh"
#include "KTMultiTrackEventData.hh"


namespace Katydid
{
    KTLOGGER(evlog, "KTCavityEventProcessing");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTCavityEventProcessing, "cavity-event-processing");

    KTCavityEventProcessing::KTCavityEventProcessing(const std::string& name) :
            KTProcessor(name),
            fProcessedCavityEventSignal("proc-cavity-event", this),
            fEventSlot("mt-event", this, &KTCavityEventProcessing::AnalyzeEvent, &fProcessedCavityEventSignal)
    {
    }

    KTCavityEventProcessing::~KTCavityEventProcessing()
    {
    }

    bool KTCavityEventProcessing::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;
        
        return true;
    }

    bool KTCavityEventProcessing::AnalyzeEvent( KTMultiTrackEventData& mtEventData )
    {
        KTDEBUG(evlog, "Entered Analyze Event Function");

        return true;
    }

} // namespace Katydid
