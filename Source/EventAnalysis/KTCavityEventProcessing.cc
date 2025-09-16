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

#include <algorithm>


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

        // initializing a map to store processed tracks in MPT groupings
        std::map<int, std::vector<AllTrackData>> mptsBySequence;

        for (TrackSetCIt trackIt = mtEventData.GetTracksBegin(); trackIt != mtEventData.GetTracksEnd(); ++trackIt)
        {
            const AllTrackData& track = *trackIt;
            int eventSequenceID = track.fProcTrack.GetEventSequenceID();
            mptsBySequence[eventSequenceID].push_back(track);
        }

        // iterating through MPTs and sorting bands by their start frequency used later for assigning track sideband order
        for (auto& [eventSeqID, tracksInMPT] : mptsBySequence)
        {
            KTDEBUG(evlog, "MultiPeakTrack (Event Sequence ID: " << eventSeqID << ")");
            
            std::multimap<double, const AllTrackData*> sortedMPTBands;
            for (const auto& track : tracksInMPT)
            {
                sortedMPTBands.emplace(track.fProcTrack.GetStartFrequency(), &track);
            }

            for (const auto& [startFreq, trackPtr] : sortedMPTBands)
            {
                KTDEBUG(evlog, "TrackID: " << trackPtr->fProcTrack.GetTrackID());
                KTDEBUG(evlog, "StartFrequency: " << startFreq);
                KTDEBUG(evlog, "EndFrequency: " << trackPtr->fProcTrack.GetEndFrequency());
                KTDEBUG(evlog, "StartTimeInRunC: " << trackPtr->fProcTrack.GetStartTimeInRunC());
                KTDEBUG(evlog, "EndTimeInRunC: " << trackPtr->fProcTrack.GetEndTimeInRunC());
            }

        }

        return true;
    }

} // namespace Katydid
