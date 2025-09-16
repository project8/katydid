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
#include "KTProcessedCavityEventData.hh"

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
        KTINFO(evlog, "Beginning start cyclotron frequency reconstruction of first multi-peak-tracks in events.")

        // Storing processed tracks in map by MPT groupings
        std::map<int, std::vector<AllTrackData>> mptsBySequence;
        for (TrackSetCIt trackIt = mtEventData.GetTracksBegin(); trackIt != mtEventData.GetTracksEnd(); ++trackIt)
        {
            const AllTrackData& track = *trackIt;
            int eventSequenceID = track.fProcTrack.GetEventSequenceID();
            mptsBySequence[eventSequenceID].push_back(track);
        }

        // Initializing parameters used in cyclotron frequency reconstruction algorithm
        double eventStartTime = mtEventData.GetStartTimeInRunC();
        double initialCyclotronFrequency = -1;
        float trackClassRelPowThresh_3Bands = 0.7;
        KTDEBUG(evlog, "Relative NUP threshold for classification of bands in 3 band multi-peak-tracks is " << trackClassRelPowThresh_3Bands);


        // Iterating through MPTs
        for (auto& [eventSeqID, tracksInMPT] : mptsBySequence)
        {
            // Performing cyclotron reconstruction only on first MPT in event 
            if (eventSeqID == 0)
            {
                KTDEBUG(evlog, "Looking at MultiPeakTrack with Event Sequence ID: " << eventSeqID);
            
                // Sorting bands in MPT by increasing order of start frequency used for track classification.
                // Finding the maximum TotalTrackNUP out of all bands in MPT used for track classification of MPTs with 3 bands to decide between [-4, -2, 0] and [-2, 0, 2] topologies
                double maxTotNUP = 0;
                std::vector<const AllTrackData*> sortedMPTBands;
                for (const auto& track : tracksInMPT)
                {
                    sortedMPTBands.push_back(&track);
                    if (track.fProcTrack.GetTotalTrackNUP() >= maxTotNUP) {maxTotNUP=track.fProcTrack.GetTotalTrackNUP();}
                }
                std::sort(sortedMPTBands.begin(), sortedMPTBands.end(), 
                    [](const AllTrackData* a, const AllTrackData* b)
                    {
                        return a->fProcTrack.GetStartFrequency() < b->fProcTrack.GetStartFrequency();
                    });

                KTDEBUG("Max Total NUP out of all bands in first track : " << maxTotNUP);

                /*
                for (const AllTrackData* trackPtr : sortedMPTBands)
                {
                    KTDEBUG(evlog, "TrackID: " << trackPtr->fProcTrack.GetTrackID());
                    KTDEBUG(evlog, "TotalTrackNUP: " << trackPtr->fProcTrack.GetTotalTrackNUP());
                    KTDEBUG(evlog, "StartFrequency: " << trackPtr->fProcTrack.GetStartFrequency());
                    KTDEBUG(evlog, "EndFrequency: " << trackPtr->fProcTrack.GetEndFrequency());
                    KTDEBUG(evlog, "StartTimeInRunC: " << trackPtr->fProcTrack.GetStartTimeInRunC());
                    KTDEBUG(evlog, "EndTimeInRunC: " << trackPtr->fProcTrack.GetEndTimeInRunC());
                }
                */

                // Classifying bands in MPT. Number of bands determines possible topologies as follows
                // Bands |        Topologies
                //   2   |          [-2, 0]
                //   3   | [-4, -2, 0] OR [-2, 0, 2]
                //   4   |      [-4, -2, 0, 2]
                // Integers in topologies refer to sideband "order" (ie. -2 means second order lower sideband)
                // Inital cyclotron frequency extracted from carrier with sideband order 0 by determine its position depending on topology
                size_t numBands = sortedMPTBands.size();
                int carrierIndex = -1;

                if (numBands==0)
                {
                    KTWARN(evlog, "MPT has no tracks; I cannot analyze this for cyclotron frequency. Aborting");
                    return false;
                }
                else if (numBands==1)
                {
                    carrierIndex = 0;
                    KTDEBUG(evlog, "MPT has 1 band!");
                }
                else if (numBands==2)
                {
                    carrierIndex = 1;
                    KTDEBUG(evlog, "MPT has 2 bands! Assumed band topology = [-2, 0].")
                }
                else if (numBands==3)
                {
                    // For 3 band MPT 2 possibilities for topology. 
                    // If the relative TotalTrackNUP of the band with the highest start frequency is larger than would be expected for a 2nd order upper sideband, it is assumed to be a carrier thus fixing the topology to [-4, -2, 0]
                    // Threshold for expectation of largest possible relative TotalTrackNUP for 2nd order sidebands initialized above (will be configurable parameter soon)
                    const AllTrackData* lastTrack = sortedMPTBands[numBands-1];
                    if (maxTotNUP!=0)
                    {
                        double lastTrackRelTotalNUP = lastTrack->fProcTrack.GetTotalTrackNUP()/maxTotNUP;
                        if (lastTrackRelTotalNUP>trackClassRelPowThresh_3Bands)
                        {
                            carrierIndex = 2;
                            KTDEBUG(evlog, "MPT has 3 bands! Relative band threshold classification determined band topology = [-4, -2, 0].")
                        }
                        else
                        {
                            carrierIndex = 1;
                            KTDEBUG(evlog, "MPT has 3 bands! Relative band threshold classification determined band topology = [-2, 0, 2].")

                        }
                        
                    }
                    else
                    {
                        KTWARN(evlog, "MPT has 3 bands; however, the band with the maximum total track NUP is 0 and I am unable to calculate the relative power of bands. Aborting");
                        return false;
                    }
                }
                else if (numBands==4)
                {
                    carrierIndex = 2;
                    KTDEBUG(evlog, "MPT has 4 bands! Assumed band topology = [-4, -2, 0, 2].");
                }
                else if (numBands>=5)
                {
                    KTWARN(evlog, "MPT has 5 or more bands; I don't know how to analyze this. Aborting");
                    return false;
                }

                // Calculating initial cyclotron frequency of event if correctly determined carrier location in MPT
                if (carrierIndex!=-1)
                {
                    const AllTrackData* carrier = sortedMPTBands[carrierIndex];
                    initialCyclotronFrequency = carrier->fProcTrack.GetSlope()*eventStartTime + carrier->fProcTrack.GetIntercept();
                }
                else
                {
                    KTWARN(evlog, "Faulty index for carrier returned. Aborting")
                    return false;
                }

            }

        }


        // Create and fill new data object
        KTProcessedCavityEventData& procEvent = mtEventData.Of<KTProcessedCavityEventData>();

        procEvent.SetComponent(mtEventData.GetComponent());
        procEvent.SetAcquisitionID(mtEventData.GetAcquisitionID());
        procEvent.SetEventID(mtEventData.GetEventID());
        procEvent.SetTotalEventSequences(mtEventData.GetTotalEventSequences());

        KTINFO(evlog, "Found initial cyclotron frequency: " << initialCyclotronFrequency)
        procEvent.SetInitialCyclotronFrequency(initialCyclotronFrequency);

        return true;
    }

} // namespace Katydid
