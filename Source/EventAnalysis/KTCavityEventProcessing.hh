/*
 * KTCavityEventProcessing.hh
 *
 *  Created on: Sep 11, 2025
 *      Author: juniorpe
 */

#ifndef KTCAVITYEVENTPROCESSING_HH_
#define KTCAVITYEVENTPROCESSING_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTSlot.hh"
#include "KTMemberVariable.hh"
#include "KTProcessedTrackData.hh"
#include "KTProcessedCavityEventData.hh"

namespace Katydid
{
    /*
     @class KTCavityEventProcessing
     @author J. I. Pena
     @brief Reconstructs axial and cyclotron frequency for all multi-peak-tracks in an event
     
     @details
     Iterates through all MPTs in an event by sequence ID
     Reconstructs axial frequency by calculating frequency distance between different bands in MPTs at distinct band start and end times
        Assumes an axially symmetric trap and cavity mode map such that sideband separation is 2*n*f_ax
     Reconstructs cyclotron frequency by classifying MPT topology based on relative power of bands to the band in the MPT with the highest power
         Relative power threshold parameter used 2 decide between 2 classes in the cases of 2 and 3 band MPTs

     Available configuration values:
     - "max-rel-power-thresh": float -- Relative power threshold which is criteria in classifying bands in MPTs

     Slots:
     - "mt-event": void (Nymph::KTDataPtr) -- Analyzes a multi-track-event; Requires KTMultiTrackEventData; Adds nothing

     Signals:
     - "proc-cavity-event": void (Nymph::KTDataPtr) -- Emitted upon successful determination of cyclotron frequency; Guarantees KTProcessedCavityEventData
    */

    class KTMultiTrackEventData;

    class KTCavityEventProcessing : public Nymph::KTProcessor
    {
        public:
            KTCavityEventProcessing(const std::string& name = "cavity-event-processing");
            virtual ~KTCavityEventProcessing();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(float, MaxRelPowerThresh);

        public:
            bool AnalyzeEvent( KTMultiTrackEventData& mtEventData );
            
            bool ReconstructCyclotronFrequency(const std::vector<AllTrackData>& tracksInMPT, KTProcessedCavityEventData& procEvent, double& outStartCyclotronFrequency) const;
            bool ReconstructAxialFrequency(const std::vector<AllTrackData>& tracksInMPT, double& outAxialFrequency) const;


            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fProcessedCavityEventSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTMultiTrackEventData > fEventSlot;

    };

}

#endif /* KTCAVITYEVENTPROCESSING_HH_ */
