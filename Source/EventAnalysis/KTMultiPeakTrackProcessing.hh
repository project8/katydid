/*
 * KTMultiPeakTrackProcessing.hh
 *
 *  Created on: Dec 13, 2016
 *      Author: ezayas
 */

#ifndef KTMULTIPEAKTRACKPROCESSING_HH_
#define KTMULTIPEAKTRACKPROCESSING_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTSlot.hh"

namespace Katydid
{
    /*
     @class KTMultiPeakTrackProcessing
     @author E. Zayas
     @brief Attempts to assign axial frequency to MPT structure
     @details
     Combines classification info from tracks in a MPT group to determine the mainband track and its axial frequency

     Available configuration values:
     (none)

     Slots:
     - "mpt": void (Nymph::KTDataPtr) -- Analyzes a multi-peak track with classified tracks; Requires KTMultiPeakTrackData; Adds nothing

     Signals:
     - "proc-mpt": void (Nymph::KTDataPtr) -- Emitted upon successful determination of the mainband track and axial frequency; Guarantees KTProcessedMPTData
    */

    class KTMultiPeakTrackData;

    class KTMultiPeakTrackProcessing : public Nymph::KTProcessor
    {
        public:
            KTMultiPeakTrackProcessing(const std::string& name = "mpt-processing");
            virtual ~KTMultiPeakTrackProcessing();

            bool Configure(const scarab::param_node* node);

        public:
            bool AnalyzeMPT( KTMultiPeakTrackData& mptData );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fProcessedMPTSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataOneType< KTMultiPeakTrackData > fMPTSlot;

    };

}

#endif /* KTMULTIPEAKTRACKPROCESSING_HH_ */
