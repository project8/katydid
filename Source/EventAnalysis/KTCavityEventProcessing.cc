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
#include <set>


namespace Katydid
{
    KTLOGGER(evlog, "KTCavityEventProcessing");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTCavityEventProcessing, "cavity-event-processing");

    KTCavityEventProcessing::KTCavityEventProcessing(const std::string& name) :
            KTProcessor(name),
            fMaxRelPowerThresh(0.),
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

        SetMaxRelPowerThresh(node->get_value("max-rel-power-thresh", GetMaxRelPowerThresh()));
        
        return true;
    }

    bool KTCavityEventProcessing::AnalyzeEvent( KTMultiTrackEventData& mtEventData )
    {
        // Create and fill new data object
        KTProcessedCavityEventData& procEvent = mtEventData.Of<KTProcessedCavityEventData>();

        procEvent.ClearProcessedEvent();

        procEvent.SetComponent(mtEventData.GetComponent());
        procEvent.SetAcquisitionID(mtEventData.GetAcquisitionID());
        procEvent.SetEventID(mtEventData.GetEventID());
        procEvent.SetTotalEventSequences(mtEventData.GetTotalEventSequences());
        procEvent.SetFirstTrackStartCyclotronFrequency(-1.);
        procEvent.SetFirstTrackAxialFrequency(-1.);


        // Storing processed tracks in map by MPT groupings
        std::map<int, std::vector<AllTrackData>> mptsBySequence;
        for (TrackSetCIt trackIt = mtEventData.GetTracksBegin(); trackIt != mtEventData.GetTracksEnd(); ++trackIt)
        {
            const AllTrackData& track = *trackIt;
            int eventSequenceID = track.fProcTrack.GetEventSequenceID();
            mptsBySequence[eventSequenceID].push_back(track);
        }

        KTINFO(evlog, "Event Start Time : " << mtEventData.GetStartTimeInRunC());

        // Iterating through MPTs
        for (auto& [eventSeqID, tracksInMPT] : mptsBySequence)
        {
            

            KTDEBUG(evlog, "Looking at MultiPeakTrack with Event Sequence ID: " << eventSeqID);

            double reconstructedStartCyclotronFrequency = -1.0;
            if (!ReconstructCyclotronFrequency(tracksInMPT, procEvent, reconstructedStartCyclotronFrequency))
            {
                KTWARN(evlog, "Cyclotron frequency reconstruction failed for MPT with Event Sequence ID: " << eventSeqID);
                if(eventSeqID==0) {return false;}
                continue;
            }

            double reconstructedAxialFrequency = -1.0;
            if (!ReconstructAxialFrequency(tracksInMPT, reconstructedAxialFrequency))
            {
                KTWARN(evlog, "Axial frequency reconstruction failed.");
            }
            
            // Storing reconstructed frequencies for only first multi-peak-track
            if (eventSeqID == 0)
            {
                KTINFO(evlog, "Found first track initial cyclotron frequency: " << reconstructedStartCyclotronFrequency);
                procEvent.SetFirstTrackStartCyclotronFrequency(reconstructedStartCyclotronFrequency);

                KTINFO(evlog, "Found first track axial frequency: " << reconstructedAxialFrequency);
                procEvent.SetFirstTrackAxialFrequency(reconstructedAxialFrequency);
            }

            procEvent.AddProcessedMPT(eventSeqID, reconstructedStartCyclotronFrequency, reconstructedAxialFrequency);
            

        }

        return true;
    }

    bool KTCavityEventProcessing::ReconstructCyclotronFrequency(const std::vector<AllTrackData>& tracksInMPT, KTProcessedCavityEventData& procEvent, double& outStartCyclotronFrequency) const
    {
        outStartCyclotronFrequency = -1.0;

        if (tracksInMPT.empty())
        {
            KTWARN(evlog, "MPT has no tracks; cannot reconstruct cyclotron frequency.");
            return false;
        }

        KTINFO(evlog, "Beginning start cyclotron frequency reconstruction of MPT.");
        KTDEBUG(evlog, "Relative NUP threshold for classification of bands in 3 band multi-peak-tracks is " << this->fMaxRelPowerThresh); //float fMaxRelPowerThresh = 0.1; // Tuned parameter for CCA simulation data
    
        // Sorting bands in MPT by increasing order of start frequency used for track classification.
        // Finding the maximum TotalTrackNUP out of all bands in MPT used for track classification of MPTs with 3 bands to decide between [-4, -2, 0] and [-2, 0, 2] topologies
        // Calculating MPT start time for start cyclotron frequency reconstruction
        double maxTotNUP = 0;
        double startTime = tracksInMPT.front().fProcTrack.GetStartTimeInRunC();
        KTDEBUG(evlog, "Initial MPT start time guess : " << startTime);
        std::vector<const AllTrackData*> sortedMPTBands;
        size_t numBands = tracksInMPT.size();
        sortedMPTBands.reserve(numBands);

        for (const auto& track : tracksInMPT)
        {
            sortedMPTBands.push_back(&track);
            if (track.fProcTrack.GetTotalTrackNUP() > maxTotNUP) {maxTotNUP=track.fProcTrack.GetTotalTrackNUP();}
            if (track.fProcTrack.GetStartTimeInRunC() < startTime) {startTime=track.fProcTrack.GetStartTimeInRunC();}
        }
        std::sort(sortedMPTBands.begin(), sortedMPTBands.end(), 
            [](const AllTrackData* a, const AllTrackData* b)
            {
                return a->fProcTrack.GetStartFrequency() < b->fProcTrack.GetStartFrequency();
            });

        KTDEBUG(evlog, "Max Total Track NUP out of all bands in MPT : " << maxTotNUP);
        KTDEBUG(evlog, "MPT start time : " << startTime);

        // Classifying bands in MPT. Number of bands determines possible topologies as follows
        // Bands |         Topologies
        //   2   |     [-2, 0] OR [0, 2]
        //   3   | [-4, -2, 0] OR [-2, 0, 2]
        //   4   |      [-4, -2, 0, 2]
        // Integers in topologies refer to sideband "order" (ie. -2 means second order lower sideband)
        // Inital cyclotron frequency extracted from carrier with sideband order 0 by determine its position depending on topology
        int carrierIndex = -1;
        std::vector<int> bandClassification;

        if (numBands==1)
        {
            carrierIndex = 0;
            bandClassification = {0};
            KTDEBUG(evlog, "MPT has 1 band!");
        }
        else if (numBands==2)
        {
            // For 2 band MPT 2 possibilities for topology. 
            // If the relative TotalTrackNUP of the band with the highest start frequency is smaller than would be expected for a carrier, assume 2nd order lower sideband was missed and assign [0, 1]
            // Threshold for expectation of largest possible relative TotalTrackNUP is the configurable parameter
            const AllTrackData* lastTrack = sortedMPTBands[numBands-1];
            if (maxTotNUP!=0)
            {
                double lastTrackRelTotalNUP = lastTrack->fProcTrack.GetTotalTrackNUP()/maxTotNUP;
                if (lastTrackRelTotalNUP> this->fMaxRelPowerThresh)
                {
                    carrierIndex = 1;
                    bandClassification = {-2, 0};
                    KTDEBUG(evlog, "MPT has 2 bands! Relative band threshold classification determined band topology = [-2, 0].");
                }
                else
                {
                    carrierIndex = 0;
                    bandClassification = {0, 2};
                    KTDEBUG(evlog, "MPT has 2 bands! Relative band threshold classification determined band topology = [0, 2].");

                }
                
            }
            else
            {
                KTWARN(evlog, "MPT has 2 bands; however, the band with the maximum total track NUP is 0 and I am unable to calculate the relative power of bands. Aborting");
                return false;
            }
        }
        else if (numBands==3)
        {
            // For 3 band MPT 2 possibilities for topology. 
            // If the relative TotalTrackNUP of the band with the highest start frequency is larger than would be expected for a 2nd order upper sideband, it is assumed to be a carrier thus fixing the topology to [-4, -2, 0]
            const AllTrackData* lastTrack = sortedMPTBands[numBands-1];
            if (maxTotNUP!=0)
            {
                double lastTrackRelTotalNUP = lastTrack->fProcTrack.GetTotalTrackNUP()/maxTotNUP;
                if (lastTrackRelTotalNUP> this->fMaxRelPowerThresh)
                {
                    carrierIndex = 2;
                    bandClassification = {-4, -2, 0};
                    KTDEBUG(evlog, "MPT has 3 bands! Relative band threshold classification determined band topology = [-4, -2, 0].");
                }
                else
                {
                    carrierIndex = 1;
                    bandClassification = {-2, 0, 2};
                    KTDEBUG(evlog, "MPT has 3 bands! Relative band threshold classification determined band topology = [-2, 0, 2].");

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
            bandClassification = {-4, -2, 0, 2};
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
            outStartCyclotronFrequency = carrier->fProcTrack.GetSlope()*startTime + carrier->fProcTrack.GetIntercept();
            KTDEBUG(evlog, "Found MPT start cyclotron frequency: " << outStartCyclotronFrequency);
        }
        else
        {
            KTWARN(evlog, "Faulty index for carrier returned. Aborting");
            return false;
        }

        if (bandClassification.size() != numBands)
        {
            KTWARN(evlog, "Band classification size (" << bandClassification.size()<< ") is different from number of bands found in MPT (" << numBands << "). Aborting");
            return false;
        }


        const int seqID = sortedMPTBands.front()->fProcTrack.GetEventSequenceID();
        KTDEBUG(evlog, "Adding individual track sideband classification: (trackID, classification)");

        // Clearing values for first MPT band classification
        if (seqID==0)
        {
            procEvent.ClearFirstTrackBandInfo();
        }

        for (std::size_t i = 0; i < numBands; ++i)
        {
            const auto* track = sortedMPTBands[i];
            if (track->fProcTrack.GetEventSequenceID() != seqID)
            {
                KTWARN(evlog, "Sorted bands contain multiple EventSequenceIDs(expected " << seqID << "), something upstream is wrong. Aborting");
                return false;
            }
            const int trackID = track->fProcTrack.GetTrackID();
            const int classification = bandClassification[i];

            procEvent.AddClassificationData(trackID, seqID, classification);
            KTDEBUG(evlog, "( " << trackID << ", " << classification << " )");

            if (seqID == 0)
            {
                procEvent.AddFirstTrackBandInfo(trackID, classification);
                KTINFO(evlog, "First MPT Band Classifications (trackID, bandClassification): (" << trackID << ", " << classification << " )");
            }
        }

        return true;
    }

    bool KTCavityEventProcessing::ReconstructAxialFrequency(const std::vector<AllTrackData>& tracksInMPT, double& outAxialFrequency) const
    {
        outAxialFrequency = -1.0;
        if (tracksInMPT.size() <= 1)
        {
            KTINFO(evlog, "MPT has <= 1 band; axial frequency reconstruction impossible.");
            return false;
        }


        KTINFO(evlog, "Beginning axial frequency reconstruction of MPT.");
        KTDEBUG(evlog, "Attemping frequency distance calculation between bands in MPT.");
        std::set<double> timeStampSet;
        std::vector<double> frequencyDistances;

        // Calculating all unique frequency distances between tracks at their start and end times
        for (auto outerIt = tracksInMPT.begin(); outerIt != tracksInMPT.end(); ++outerIt) 
        {
            double t1StartTime = outerIt->fProcTrack.GetStartTimeInRunC();
            double t1EndTime = outerIt->fProcTrack.GetEndTimeInRunC();
            double t1StartFreq = outerIt->fProcTrack.GetStartFrequency();
            double t1EndFreq = outerIt->fProcTrack.GetEndFrequency();
            double t1Slope = outerIt->fProcTrack.GetSlope();
            double t1Intercept = outerIt->fProcTrack.GetIntercept();
            
            KTDEBUG(evlog, "Outer loop track id: " << outerIt->fProcTrack.GetTrackID());

            // Looping through track start and end times to calculate frequency distance to other tracks at those time stamps
            for (const double& timeStamp : {t1StartTime, t1EndTime})
            {
                KTDEBUG(evlog, "Time stamp: " << timeStamp)
                //Skip redundant time stamps
                if (timeStampSet.count(timeStamp) > 0) 
                {
                    KTDEBUG(evlog, "Redundant time stamp encountered. Skipping frequency distance calculation!"); 
                    continue;
                }
                //Store unique time stamps
                timeStampSet.insert(timeStamp);

                double freqDist = 0;
                double t1FreqAtTimeStamp = t1Slope*timeStamp + t1Intercept;

                for (auto innerIt = tracksInMPT.begin(); innerIt != tracksInMPT.end(); ++innerIt) 
                {
                    // Skip outer loop track
                    if (outerIt == innerIt) continue; 
                    
                    double t2StartTime = innerIt->fProcTrack.GetStartTimeInRunC();
                    double t2EndTime = innerIt->fProcTrack.GetEndTimeInRunC();
                    double t2StartFreq = innerIt->fProcTrack.GetStartFrequency();
                    double t2EndFreq = innerIt->fProcTrack.GetEndFrequency();
                    double t2Slope = innerIt->fProcTrack.GetSlope();
                    double t2Intercept = innerIt->fProcTrack.GetIntercept();


                    KTDEBUG(evlog, "Inner loop track id: " << innerIt->fProcTrack.GetTrackID())

                    // Check that time stamp is contained within other track lengths before calculating frequency distance
                    if (t2StartTime <= timeStamp && t2EndTime >= timeStamp)
                    {
                        double t2FreqAtTimeStamp = t2Slope*timeStamp + t2Intercept;
                        freqDist = std::abs(t1FreqAtTimeStamp - t2FreqAtTimeStamp);
                        frequencyDistances.push_back(freqDist);
                        KTDEBUG(evlog, "Successfully calculated frequency distance: " << freqDist)
                    }
                    else 
                    {
                        KTDEBUG(evlog, "Time stamp not contained in inner loop track length. Aborting frequency distance calculation!");
                        continue;
                    }
                }

            }
        
        }

        if (frequencyDistances.empty())
        {
            KTWARN(evlog, "No valid frequency distances found; cannot reconstruct axial frequency.");
            return false;
        }

        KTINFO(evlog, "Beginning calculation of average axial frequency from all frequency distances.");
        // Calculating average axial frequency from frequency distances between tracks
        auto minDist = std::min_element(frequencyDistances.begin(), frequencyDistances.end());
        std::vector<double> separationOrder;
        // Check if the vector is not empty and min is not zero to avoid division by zero
        if (minDist == frequencyDistances.end() || *minDist == 0.0)
        {
            KTWARN(evlog, "Cannot determine minimum frequency distance (empty or zero); cannot reconstruct axial frequency.");
            return false;
        }
        double min_val = *minDist;
        // Divide all frequency distances by the minimum frequency distance and store value
        for (double& val : frequencyDistances) {
            separationOrder.push_back(std::round(val/min_val));
        }

        if (frequencyDistances.size() != separationOrder.size()) 
        {
            KTWARN(evlog, "Error: Frequency distances vector has different size to separation order vector.");
            return false;
        }

        KTDEBUG(evlog, "Separation order of frequency distances:");
        for (std::size_t i = 0; i < separationOrder.size(); ++i)
        {
            KTDEBUG(evlog, separationOrder[i]);
        }

        double sum = 0.0;
        for (std::size_t i = 0; i < frequencyDistances.size(); ++i) 
        {
            if (separationOrder[i] == 0.0) 
            {
                KTWARN(evlog, "Error: Frequency distance division by 0 separation order.");
                return false;
            }
            sum += frequencyDistances[i] / (2*separationOrder[i]);// Assuming only even order sidebands visible!!!
            KTDEBUG(evlog, "Average axial frequency constribution " << i+1  << " : " << frequencyDistances[i] / (2*separationOrder[i]));
        }

        outAxialFrequency = sum/frequencyDistances.size();
        KTDEBUG(evlog, "Reconstructed Axial Frequency: " << outAxialFrequency);
        

        return true;
    }

} // namespace Katydid
