/*
 * KTCenterFrequencyShift.hh
 *
 *  Created on: Nov 14, 2018
 *      Author: ezayas
 */

#ifndef KTCENTERFREQUENCYSHIFT_HH_
#define KTCENTERFREQUENCYSHIFT_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTSlot.hh"

namespace Katydid
{

    class KTProcessedTrackData;

    /*
     @class KTCenterFrequencyShift
     @author E. Zayas

     @brief Shifts the start frequency by a fixed amount

     @details
     Example processor for tutorial purposes

     Configuration name: "center-frequency-shift"

     Available configuration values:
     - "center-frequency": double -- amount to shift start frequency

     Slots:
     - "track": void (Nymph::KTDataPtr) -- Shifts the start frequency of a track. Requires KTProcessedTrackData. Adds nothing.
    
     Signals:
     - "track": void (Nymph::KTDataPtr) -- Emitted upon successful shift of start frequency. Guarantees KTProcessedTrackData.
    */

    class KTCenterFrequencyShift : public Nymph::KTProcessor
    {
        public:
            KTCenterFrequencyShift(const std::string& name = "center-frequency-shift");
            virtual ~KTCenterFrequencyShift();

            bool Configure(const scarab::param_node* node);

            MEMBERVARIABLE(double, CF);

            bool ShiftCF( KTProcessedTrackData& trackData );

        private:

            //***************
            // Signals
            //***************

            Nymph::KTSignalData fTrackSignal;

            //***************
            // Slots
            //***************

            void SlotFunctionTrack( Nymph::KTDataPtr data );

    };
}

#endif /* KTCENTERFREQUENCYSHIFT_HH_ */
