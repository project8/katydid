/*
 * KTDataCutter.cc
 *
 *  Created on: Oct 13, 2016
 *      Author: ezayas
 */

#include "KTDataCutter.hh"

#include "KTLogger.hh"

#include "KTSliceHeader.hh"
#include "KTProcessedTrackData.hh"
#include "KTLinearFitResult.hh"

#include <cmath>

namespace Katydid
{
    KTLOGGER(evlog, "KTDataCutter");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTDataCutter, "data-cutter");

    KTDataCutter::KTDataCutter(const std::string& name) :
            KTProcessor(name),
            fMinSidebandSeparation(-1e12),
            fMaxSidebandSeparation(1e12),
            fMinMagnetronFreq(0.),
            fMaxMagnetronFreq(1e12),
            fMinTrackID(0),
            fMaxTrackID(1e9),
            fCutBogusTracks(false),
            fIsCut(false),
            fMinStartTimeInRunC(0.),
            fMaxStartTimeInRunC(1.),
            fMinTimeLength(0.),
            fMaxTimeLength(1.),
            fMinStartFreq(0.),
            fMaxStartFreq(1e12),
            fMinSlope(-1e15),
            fMaxSlope(1e15),
            fLinearFitSignal("fit-result", this),
            fTrackSignal("track", this)
    {
        RegisterSlot( "fit-result", this, &KTDataCutter::SlotFunctionFitResult );
        RegisterSlot( "track", this, &KTDataCutter::SlotFunctionProcessedTrack );
    }

    KTDataCutter::~KTDataCutter()
    {
    }

    bool KTDataCutter::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        SetMinSidebandSeparation(node->get_value< double >("min-sideband-separation", fMinSidebandSeparation));
        SetMaxSidebandSeparation(node->get_value< double >("max-sideband-separation", fMaxSidebandSeparation));
        SetMinMagnetronFreq(node->get_value< double >("min-magnetron-freq", fMinMagnetronFreq));
        SetMaxMagnetronFreq(node->get_value< double >("max-magnetron-freq", fMaxMagnetronFreq));
        SetMinTrackID(node->get_value< unsigned >("min-track-id", fMinTrackID));
        SetMaxTrackID(node->get_value< unsigned >("max-track-id", fMaxTrackID));
        SetCutBogusTracks(node->get_value< bool >("cut-bogus-tracks", fCutBogusTracks));
        SetIsCut(node->get_value< bool >("is-cut", fIsCut));
        SetMinStartTimeInRunC(node->get_value< double >("min-start-time-in-run-c", fMinStartTimeInRunC));
        SetMaxStartTimeInRunC(node->get_value< double >("max-start-time-in-run-c", fMaxStartTimeInRunC));
        SetMinTimeLength(node->get_value< double >("min-time-length", fMinTimeLength));
        SetMaxTimeLength(node->get_value< double >("max-time-length", fMaxTimeLength));
        SetMinStartFreq(node->get_value< double >("min-start-freq", fMinStartFreq));
        SetMaxStartFreq(node->get_value< double >("max-start-freq", fMaxStartFreq));
        SetMinSlope(node->get_value< double >("min-slope", fMinSlope));
        SetMaxSlope(node->get_value< double >("max-slope", fMaxSlope));
        
        return true;
    }

    bool KTDataCutter::CutLinearFitResult( KTLinearFitResult& fitData )
    {
        if( abs( fitData.GetSidebandSeparation( 0 ) ) < fMinSidebandSeparation )
        {
            return false;
        }
        if( abs( fitData.GetSidebandSeparation( 0 ) ) > fMaxSidebandSeparation )
        {
            return false;
        }

        if( fitData.GetFFT_peak( 0 ) < fMinMagnetronFreq )
        {
            return false;
        }
        if( fitData.GetFFT_peak( 0 ) > fMaxMagnetronFreq )
        {
            return false;
        }

        return true;
    }

    bool KTDataCutter::CutProcessedTrack( KTProcessedTrackData& trackData )
    {
        if( trackData.GetTrackID() < fMinTrackID )
        {
            return false;
        }
        if( trackData.GetTrackID() > fMaxTrackID )
        {
            return false;
        }

        if( fCutBogusTracks && trackData.GetIsCut() != fIsCut )
        {
            return false;
        }

        if( trackData.GetStartTimeInRunC() < fMinStartTimeInRunC )
        {
            return false;
        }
        if( trackData.GetStartTimeInRunC() > fMaxStartTimeInRunC )
        {
            return false;
        }

        if( trackData.GetTimeLength() < fMinTimeLength )
        {
            return false;
        }
        if( trackData.GetTimeLength() > fMaxTimeLength )
        {
            return false;
        }

        if( trackData.GetStartFrequency() < fMinStartFreq )
        {
            return false;
        }
        if( trackData.GetStartFrequency() > fMaxStartFreq )
        {
            return false;
        }

        if( trackData.GetSlope() < fMinSlope )
        {
            return false;
        }
        if( trackData.GetSlope() > fMaxSlope )
        {
            return false;
        }

        return true;
    }

} // namespace Katydid
