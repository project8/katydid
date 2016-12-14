/*
 * KTMultiPeakTrackProcessing.cc
 *
 *  Created on: Dec 13, 2016
 *      Author: ezayas
 */

#include "KTMultiPeakTrackProcessing.hh"

#include "KTLogger.hh"

namespace Katydid
{
    KTLOGGER(evlog, "KTMultiPeakTrackProcessing");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTMultiPeakTrackProcessing, "track-classifier");

    KTMultiPeakTrackProcessing::KTMultiPeakTrackProcessing(const std::string& name) :
            KTProcessor(name),
            fProcessedMPTSignal("proc-mpt", this)
    {
        RegisterSlot( "mpt", this, &KTMultiPeakTrackProcessing::SlotFunctionMPTData );
    }

    KTMultiPeakTrackProcessing::~KTMultiPeakTrackProcessing()
    {
    }

    bool KTMultiPeakTrackProcessing::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;
        
        return true;
    }

    bool KTMultiPeakTrackProcessing::AnalyzeMPT( KTMultiPeakTrackData& mptData )
    {
        int nMainband = 0;
        TrackSetCItSet allTracks = mptData.GetMPTrack().fTrackRefs;

        // Determine number of mainband tracks
        for( TrackSetCItSet::iterator it = allTracks.begin(); it != allTracks.end(); ++it)
        {
            if( (*it)->GetMainband() )
            {
                ++nMainband;
            }
        }
        KTDEBUG(evlog, "Determined number of mainband tracks = " << nMainband);

        // If there is not exactly 1 mainband track, abort
        if( nMainband != 1 )
        {
            KTWARN(evlog, "This MPT has 0 or more than 1 track identified as mainband; I cannot analyze this. Aborting");
            return false;
        }

        // Create and fill new data object

        KTProcessedMPTData& procData = mptData.Of< KTProcessedMPTData >();

        procData.SetComponent( mptData.GetComponent() );
        procData.SetAxialFrequency( 0. );

        for( TrackSetCItSet::iterator it = allTracks.begin(); it != allTracks.end(); ++it)
        {
            if( (*it)->GetMainband() )
            {
                procData.SetMainTrack( **it );
            }
        }

        // Determine multiplicity
        int mult = mptData.GetMultiplicity();
        KTDEBUG(evlog, "Determined multiplicity = " << mult);

        if( mult > 2 )
        {
            KTWARN(evlog, "I don't yet have the logic to deal with 3+ multiplicity events; the axial frequency will be zero");
            return true;
        }
        if( mult == 1 )
        {
            KTWARN(evlog, "MPT only has one track, and it is mainband; the axial frequency will be zero");
            return true;
        }

        // At this point the multiplicity must be exactly 1
        if( mult != 1 )
        {
            KTWARN(evlog, "Something went wrong determining the multiplicity. Aborting");
            return false;
        }

        // Determine mainband and sideband frequencies
        double mainbandFreq = 0.;
        double sidebandFreq = 0.;
        for( TrackSetCItSet::iterator it = allTracks.begin(); it != allTracks.end(); ++it)
        {
            if( (*it)->GetMainband() )
            {
                mainbandFreq = (*it)->GetStartFrequency();
                KTDEBUG(evlog, "Set mainband frequency to " << mainbandFreq);
            }
            else
            {
                sidebandFreq = (*it)->GetStartFrequency();
                KTDEBUG(evlog, "Set sideband frequency to " << sidebandFreq);
            }
        }

        if( mainbandFreq == 0. || sidebandFreq == 0. )
        {
            KTERROR(evlog, "Could not determine mainband and sideband frequencies");
            return false;
        }

        // Set axial frequency

        double axialFreq = std::abs( mainbandFreq - sidebandFreq );
        KTINFO(evlog, "Found axial frequency: " << axialFreq);

        procData.SetAxialFrequency( axialFreq );

        return true;
    }

} // namespace Katydid