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
#include "KTLogger.hh"


namespace Katydid
{
    
    KTLOGGER(avlog_hh, "KTRPClassifier.hh");

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
            bool FillRPTrack( KTProcessedTrackData& trackData, KTPowerFitData& pfData, KTRPTrackData& rpTrackData );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fRPTrackSignal;

            //***************
            // Slots
            //***************

        private:
            void SlotFunctionTrack( Nymph::KTDataPtr data );

    };

    void KTRPClassifier::SlotFunctionTrack( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTProcessedTrackData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTProcessedTrackData >!");
            return;
        }

        if (! data->Has< KTPowerFitData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTPowerFitData >!");
            return;
        }
        
        // Call function
        if( !FillRPTrack( data->Of< KTProcessedTrackData >(), data->Of< KTPowerFitData >(), data->Of< KTRPTrackData >() ) )
        {
            KTERROR(avlog_hh, "Something went wrong while analyzing data!");
            return;
        }

        // Emit signal
        fRPTrackSignal( data );
    
        return;
    }

}

#endif /* KTRPCLASSIFIER_HH_ */
