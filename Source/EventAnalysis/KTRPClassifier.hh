/*
 * KTRPClassifier.hh
 *
 *  Created on: Nov 9, 2017
 *      Author: ezayas
 */

#ifndef KTRPCLASSIFIER_HH_
#define KTRPCLASSIFIER_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTSlot.hh"

namespace Katydid
{

    class KTPowerFitData;
    class KTProcessedTrackData;
    class KTRPTrackData;

    /*
     @class KTRPClassifier
     @author E. Zayas

     @brief Combines track with rotate-and-project results

     @details
     Baby classifier processor

     Configuration name: "rp-classifier"

     Available configuration values:
     (none)

     Slots:
     - "track": void (Nymph::KTDataPtr) -- Adds KTRPTrackData; Requires KTProcessedTrackData and KTPowerFitData

     Signals:
     - "rp-track": void (Nymph::KTDataPtr) -- Guarantees KTRPTrackData
    */

    class KTRPClassifier : public Nymph::KTProcessor
    {
        public:
            KTRPClassifier(const std::string& name = "rp-classifier");
            virtual ~KTRPClassifier();

            bool Configure(const scarab::param_node* node);

        public:
            bool FillRPTrack( KTProcessedTrackData& trackData, KTPowerFitData& pfData );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fRPTrackSignal;

            //***************
            // Slots
            //***************

        private:
            Nymph::KTSlotDataTwoTypes< KTProcessedTrackData, KTPowerFitData > fTrackSlot;

    };

}

#endif /* KTRPCLASSIFIER_HH_ */
