/*
 * KTCenterFrequencyShift.cc
 *
 *  Created on: Nov 14, 2018
 *      Author: ezayas
 */

#include "KTCenterFrequencyShift.hh"

#include "KTProcessedTrackData.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(evlog, "KTCenterFrequencyShift");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTCenterFrequencyShift, "center-frequency-shift");

    KTCenterFrequencyShift::KTCenterFrequencyShift(const std::string& name) :
            KTProcessor(name),
            fCF(0.),
            fTrackSignal("track", this)
    {
        RegisterSlot( "track", this, &KTCenterFrequencyShift::SlotFunctionTrack );
    }

    KTCenterFrequencyShift::~KTCenterFrequencyShift()
    {
    }

    bool KTCenterFrequencyShift::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetCF(node->get_value< double >("center-frequency", fCF));
        
        return true;
    }

    bool KTCenterFrequencyShift::ShiftCF( KTProcessedTrackData& trackData )
    {
        double oldFreq = trackData.GetStartFrequency();
        double shift = GetCF();

        trackData.SetStartFrequency( oldFreq + shift );

        return true;
    }

    void KTCenterFrequencyShift::SlotFunctionTrack( Nymph::KTDataPtr data )
    {
        if (! data->Has< KTProcessedTrackData >())
        {
            KTERROR(evlog, "Data not found with type < KTProcessedTrackData >!");
            return;
        }

        if( ! ShiftCF( data->Of< KTProcessedTrackData >() ) )
        {
            KTERROR(evlog, "Analysis failed.");
            return;
        }

        fTrackSignal( data );
    }

} // namespace Katydid
