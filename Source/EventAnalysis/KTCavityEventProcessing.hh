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
     @brief Assigns initial cyclotron frequency for events from symmetric-trap CCA cavity TE011 data
     @details
     

     Available configuration values:
     - "max-rel-power-thresh": float -- Relative power threshold which is criteria in classifying bands in MPTs

     Slots:
     - "mt-event": void (Nymph::KTDataPtr) -- Analyzes a multi-track-event; Requires KTMultiPeakEventData; Adds nothing

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
