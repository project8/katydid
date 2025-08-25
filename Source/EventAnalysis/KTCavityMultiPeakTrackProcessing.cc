/*
 * KTCavityMultiPeakTrackProcessing.cc
 *
 *  Created on: Aug 19, 2025
 *      Author: juniorpe
 */

#include "KTCavityMultiPeakTrackProcessing.hh"

#include "KTLogger.hh"
#include "KTProcessedTrackData.hh"
#include "KTProcessedCavityMPTData.hh"
#include "KTMultiTrackEventData.hh"

#include <cmath>
#include <set>
#include <algorithm>

namespace Katydid
{
    KTLOGGER(evlog, "KTCavityMultiPeakTrackProcessing");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTCavityMultiPeakTrackProcessing, "cavity-mpt-processing");

    KTCavityMultiPeakTrackProcessing::KTCavityMultiPeakTrackProcessing(const std::string& name) :
            KTProcessor(name),
            fProcessedCavityMPTSignal("proc-cavity-mpt", this),
            fMPTSlot("mpt", this, &KTCavityMultiPeakTrackProcessing::AnalyzeMPT, &fProcessedCavityMPTSignal)
    {
    }

    KTCavityMultiPeakTrackProcessing::~KTCavityMultiPeakTrackProcessing()
    {
    }

    bool KTCavityMultiPeakTrackProcessing::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;
        
        return true;
    }

    bool KTCavityMultiPeakTrackProcessing::AnalyzeMPT( KTMultiPeakTrackData& mptData )
    {
        TrackSetCItSet allTracks = mptData.GetMPTrack().fTrackRefs;

        // Determine multiplicity
        int mult = mptData.GetMultiplicity();
        KTDEBUG(evlog, "Determined multiplicity = " << mult);

        if(mult <= 1)
        {
            KTWARN(evlog, "MPT only has one or no tracks; I cannot analyze this for axial frequency. Aborting");
            return false;
        }

        // Create and fill new data object

        KTProcessedCavityMPTData& procData = mptData.Of< KTProcessedCavityMPTData >();

        procData.SetComponent( mptData.GetComponent() );
        procData.SetAxialFrequency( 0. );

        double axialFreq = 0.;
        std::set<double> timeStampSet;
        std::vector<double> frequencyDistances;

        KTINFO(evlog, "Beginning axial frequency reconstruction. Attemping frequency distance calculation between bands in mpt.")
        // Calculating all unique frequency distances between tracks at their start and end times
        for (auto outerIt = allTracks.begin(); outerIt != allTracks.end(); ++outerIt) 
        {
            double t1StartTime = (*outerIt)->fProcTrack.GetStartTimeInRunC();
            double t1EndTime = (*outerIt)->fProcTrack.GetEndTimeInRunC();
            double t1StartFreq = (*outerIt)->fProcTrack.GetStartFrequency();
            double t1EndFreq = (*outerIt)->fProcTrack.GetEndFrequency();
            double t1Slope = (*outerIt)->fProcTrack.GetSlope();
            double t1Intercept = (*outerIt)->fProcTrack.GetIntercept();
            
            KTDEBUG(evlog, "Outer loop track id: " << (*outerIt)->fProcTrack.GetTrackID())

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

                for (auto innerIt = allTracks.begin(); innerIt != allTracks.end(); ++innerIt) 
                {
                    // Skip outer loop track
                    if (outerIt == innerIt) continue; 
                    
                    double t2StartTime = (*innerIt)->fProcTrack.GetStartTimeInRunC();
                    double t2EndTime = (*innerIt)->fProcTrack.GetEndTimeInRunC();
                    double t2StartFreq = (*innerIt)->fProcTrack.GetStartFrequency();
                    double t2EndFreq = (*innerIt)->fProcTrack.GetEndFrequency();
                    double t2Slope = (*innerIt)->fProcTrack.GetSlope();
                    double t2Intercept = (*innerIt)->fProcTrack.GetIntercept();


                    KTDEBUG(evlog, "Inner loop track id: " << (*innerIt)->fProcTrack.GetTrackID())

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
                        KTDEBUG(evlog, "Time stamp not contained in inner loop track length. Aborting frequency distance calculation!")
                        continue;
                    }
                }

            }
        
        }

        KTINFO(evlog, "Beginning calculation of average axial frequency from all frequency distances.")
        // Calculating average axial frequency from frequency distances between tracks
        auto minDist = std::min_element(frequencyDistances.begin(), frequencyDistances.end());
        std::vector<double> separationOrder;
        // Check if the vector is not empty and min is not zero to avoid division by zero
        if (minDist != frequencyDistances.end() && *minDist != 0.0) 
        {
            double min_val = *minDist;
            // Divide all frequency distances by the minimum frequency distance and store value
            for (double& val : frequencyDistances) {
                separationOrder.push_back(std::round(val/min_val));
            }
        } 
        else 
        {
            KTWARN(evlog, "Frequency distances vector is empty or minimum value is zero. Aborting!")
            return false;
        }

        if (frequencyDistances.size() != separationOrder.size() || frequencyDistances.empty()) 
        {
            KTWARN(evlog, "Frequency distances vector empty or has different size to separation order vector. Aborting!")
            return false;
        }

        KTDEBUG(evlog, "Separation order of frequency distances:")
        for (std::size_t i = 0; i < separationOrder.size(); ++i)
        {
            KTDEBUG(evlog, i+1 << " : " << separationOrder[i]);
        }

        double sum = 0.0;
        for (std::size_t i = 0; i < frequencyDistances.size(); ++i) 
        {
            if (separationOrder[i] == 0.0) 
            {
                KTWARN(evlog, "Frequency distance division by 0 separation order. Aborting!")
                return false;
            }
            sum += frequencyDistances[i] / (2*separationOrder[i]);// Assuming only even order sidebands visible!!!
            KTDEBUG(evlog, "Average axial frequency constribution " << i+1  << " : " << frequencyDistances[i] / (2*separationOrder[i]));
        }
        axialFreq = sum/frequencyDistances.size();

        // Set axial frequency
        KTINFO(evlog, "Found axial frequency: " << axialFreq);
        procData.SetAxialFrequency( axialFreq );

        return true;
    }

} // namespace Katydid
