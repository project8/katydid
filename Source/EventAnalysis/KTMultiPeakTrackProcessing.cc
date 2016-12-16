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
    KT_REGISTER_PROCESSOR(KTMultiPeakTrackProcessing, "mpt-processing");

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

        // Determine multiplicity
        int mult = mptData.GetMultiplicity();
        KTDEBUG(evlog, "Determined multiplicity = " << mult);

        // If there are fewer than 2 tracks, abort
        if( mult < 2 )
        {
            KTWARN(evlog, "MPT only has one track; I cannot analyze this. Aborting");
            return false;
        }

        // If there is more than 1 mainband track, abort
        if( nMainband > 1 )
        {
            KTWARN(evlog, "This MPT has more than 1 track identified as mainband; I cannot analyze this. Aborting");
            return false;
        }

        // If there are more than 2 sidebands, abort
        if( mult - nMainband > 2 )
        {
            KTWARN(evlog, "This MPT has more than 2 tracks identified as sidebands; I cannot analyze this. Aborting");
            return false;
        }

        // Create and fill new data object

        KTProcessedMPTData& procData = mptData.Of< KTProcessedMPTData >();

        procData.SetComponent( mptData.GetComponent() );
        procData.SetAxialFrequency( 0. );

        double axialFreq = 0.;

        if( nMainband > 0 )
        {
            if( nMainband != 1 )
            {
                KTWARN(evlog, "Something went wrong determining the multiplicity. Aborting");
                return false;
            }

            // We have a mainband track, let's find it
            for( TrackSetCItSet::iterator it = allTracks.begin(); it != allTracks.end(); ++it)
            {
                if( (*it)->GetMainband() )
                {
                    procData.SetMainTrack( **it );
                }
            }

            // And calculate the axial frequency
            double avgAxialFrequency = 0.;
            for( TrackSetCItSet::iterator it = allTracks.begin(); it != allTracks.end(); ++it)
            {
                if( ! (*it)->GetMainband() )
                {
                    avgAxialFrequency += std::abs( (*it)->GetStartFrequency() - procData.GetMainTrack().GetStartFrequency() );
                }
            }
            avgAxialFrequency /= (double)(mult - nMainband);
            axialFreq = avgAxialFrequency;
        }
        else
        {
            if( mult != 2 )
            {
                KTWARN(evlog, "Something went wrong determining the multiplicity. Aborting");
                return false;
            }

            double startFreqOne = (*allTracks.begin())->GetStartFrequency();
            double startFreqTwo = (*allTracks.rbegin())->GetStartFrequency();

            // Create a new track to represent the absent mainband
            // For now I will only set the start frequency

            KTProcessedTrackData* hiddenMainband;
            hiddenMainband->SetStartFrequency( 0.5*(startFreqOne + startFreqTwo) );
            procData.SetMainTrack( *hiddenMainband );

            axialFreq = 0.5 * std::abs( startFreqOne - startFreqTwo );
        }

        // Set axial frequency
        KTINFO(evlog, "Found axial frequency: " << axialFreq);
        procData.SetAxialFrequency( axialFreq );

        return true;
    }

} // namespace Katydid
