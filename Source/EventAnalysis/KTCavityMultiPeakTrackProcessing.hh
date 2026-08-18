/*
 * KTCavityMultiPeakTrackProcessing.hh
 *
 *  Created on: Aug 19, 2025
 *      Author: juniorpe
 */

#ifndef KTCAVITYMULTIPEAKTRACKPROCESSING_HH_
#define KTCAVITYMULTIPEAKTRACKPROCESSING_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTSlot.hh"

namespace Katydid
{
    /*
     @class KTCavityMultiPeakTrackProcessing
     @author J. I. Pena
     @brief Assigns axial frequency to MPT structure for symmetric-trap cavity TE011 data
     @details
     Iterates through bands in MPT to calculate frequency distance to other bands at distinct start and end times.
     Assumes that trap is symmetric and carrier is present so that so sidebands are 2n(f_a) apart.

     Available configuration values:
     (none)

     Slots:
     - "mpt": void (Nymph::KTDataPtr) -- Analyzes a multi-peak track with > 1 band; Requires KTMultiPeakTrackData; Adds nothing

     Signals:
     - "proc-cavity-mpt": void (Nymph::KTDataPtr) -- Emitted upon successful determination of axial frequency; Guarantees KTProcessedCavityMPTData
    */

    class KTMultiPeakTrackData;

    class KTCavityMultiPeakTrackProcessing : public Nymph::KTProcessor
    {
        public:
            KTCavityMultiPeakTrackProcessing(const std::string& name = "cavity-mpt-processing");
            virtual ~KTCavityMultiPeakTrackProcessing();

            bool Configure(const scarab::param_node* node);

        public:
            bool AnalyzeMPT( KTMultiPeakTrackData& mptData );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fProcessedCavityMPTSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTMultiPeakTrackData > fMPTSlot;

    };

}

#endif /* KTCAVITYMULTIPEAKTRACKPROCESSING_HH_ */
