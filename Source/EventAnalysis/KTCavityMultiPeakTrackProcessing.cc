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

        // Create and fill new data object

        KTProcessedCavityMPTData& procData = mptData.Of< KTProcessedCavityMPTData >();

        procData.SetComponent( mptData.GetComponent() );
        procData.SetAxialFrequency( 0. );

        double axialFreq = 0.;

        // Set axial frequency
        KTINFO(evlog, "Found axial frequency: " << axialFreq);
        procData.SetAxialFrequency( axialFreq );

        return true;
    }

} // namespace Katydid
