/*
 * KTSidebandCorrection.hh
 *
 *  Created on: Oct 12, 2016
 *      Author: ezayas
 */

#ifndef KTSIDEBANDCORRECTION_HH_
#define KTSIDEBANDCORRECTION_HH_

#include "KTProcessor.hh"
#include "KTData.hh"
#include "KTEggHeader.hh"

#include "KTSlot.hh"
#include "KTLogger.hh"


namespace Katydid
{
    
    KTLOGGER(avlog_hh, "KTSidebandCorrection.hh");

    class KTLinearFitResult;
    class KTProcessedTrackData;

    /*
     @class KTSidebandCorrection
     @author E. Zayas
     @brief Corrects track frequency using sideband frequency and the Gabrielese theorem
     @details
     This processor will specifically use the results of KTLinearDensityProbe to correct a track frequency if both signal and sideband peaks are
     found with high confidence. The correction follows from the Gabrielese theorem, which states that cyclotron, axial, and magnetron frequencies
     add in quadrature (to leading order) to give the true frequency of interest in a harmonic trap.
     Configuration name: "sideband-correction"
     Available configuration values:
     (none)
     Slots:
     - "fit-result": void (Nymph::KTDataPtr) -- Adds a track and fit result for analysis; Requires KTLinearFitResult and KTProcessedTrackData; Adds nothing
     - "header": void (Nymph::KTDataPtr) -- Determines the true cyclotron frequency to correctly adjust for down-mixing from hardware; Requires KTEggHeader; Adds nothing
     Signals:
     - "track": void (Nymph::KTDataPtr) -- Emitted upon successful analysis and correction of a track; Guarantees KTProcessedTrackData
    */

    class KTSidebandCorrection : public Nymph::KTProcessor
    {
        public:
            KTSidebandCorrection(const std::string& name = "sideband-correction");
            virtual ~KTSidebandCorrection();

            bool Configure(const scarab::param_node* node);

            double GetMixingOffset() const;
            void SetMixingOffset(double freq);

        private:
            double fMixingOffset;


        public:
            bool CorrectTrack( KTLinearFitResult& fitData, KTProcessedTrackData& trackData );
            bool AssignFrequencyOffset( KTEggHeader& header );

            //***************
            // Signals
            //***************

        private:
            Nymph::KTSignalData fTrackSignal;

            //***************
            // Slots
            //***************

        private:
            void SlotFunctionFitResult( Nymph::KTDataPtr data );
            void SlotFunctionHeader( Nymph::KTDataPtr data );

    };

    inline double KTSidebandCorrection::GetMixingOffset() const
    {
        return fMixingOffset;
    }

    inline void KTSidebandCorrection::SetMixingOffset(double freq)
    {
        fMixingOffset = freq;
        return;
    }

    void KTSidebandCorrection::SlotFunctionFitResult( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTLinearFitResult >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTLinearFitResult >!");
            return;
        }
        if (! data->Has< KTProcessedTrackData >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTProcessedTrackData >!");
            return;
        }

        // Call function
        if( !CorrectTrack( data->Of< KTLinearFitResult >(), data->Of< KTProcessedTrackData >() ) )
        {
            KTERROR(avlog_hh, "Something went wrong while analyzing linear fit data!");
            return;
        }

        // Emit signal
        fTrackSignal( data );
    
        return;
    }

    void KTSidebandCorrection::SlotFunctionHeader( Nymph::KTDataPtr data )
    {
        // Standard data slot pattern:
        // Check to ensure that the required data types are present
        if (! data->Has< KTEggHeader >())
        {
            KTERROR(avlog_hh, "Data not found with type < KTEggHeader >!");
            return;
        }

        if( !AssignFrequencyOffset( data->Of< KTEggHeader >() ) )
        {
            KTERROR(avlog_hh, "Something went wrong finding the mixing offset!");
            return;
        }
    }
}

#endif /* KTSIDEBANDCORRECTION_HH_ */