/*
 * KTSidebandCorrection.cc
 *
 *  Created on: Oct 12, 2016
 *      Author: ezayas
 */

#include "KTSidebandCorrection.hh"

#include "KTLogger.hh"

#include "KTSliceHeader.hh"
#include "KTProcessedTrackData.hh"
#include "KTLinearFitResult.hh"

#include <cmath>

namespace Katydid
{
    KTLOGGER(evlog, "KTSidebandCorrection");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTSidebandCorrection, "sideband-correction");

    KTSidebandCorrection::KTSidebandCorrection(const std::string& name) :
            KTProcessor(name),
            fMixingOffset(0.),
            fTrackSignal("track", this)
    {
        RegisterSlot( "fit-result", this, &KTSidebandCorrection::SlotFunctionFitResult );
        RegisterSlot( "header", this, &KTSidebandCorrection::SlotFunctionHeader );
    }

    KTSidebandCorrection::~KTSidebandCorrection()
    {
    }

    bool KTSidebandCorrection::Configure(const scarab::param_node* node)
    {
        if (node == NULL) return false;

        return true;
    }

    bool KTSidebandCorrection::CorrectTrack( KTLinearFitResult& fitData, KTProcessedTrackData& trackData )
    {
        double startFreq = trackData.GetStartFrequency() + GetMixingOffset();
        double endFreq = trackData.GetEndFrequency() + GetMixingOffset();
        double intercept = trackData.GetIntercept() + GetMixingOffset();

        double sidebandIntercept = fitData.GetIntercept( 0 );
        double signalIntercept = fitData.GetIntercept( 1 );
        double sep = abs( signalIntercept - sidebandIntercept );
        double magnetron = fitData.GetFFT_peak( 0 );

        KTINFO(avlog_hh, "Found an axial frequency of " << sep << " Hz");
        KTINFO(avlog_hh, "Found a magnetron frequency of " << magnetron << " Hz");

        KTINFO(avlog_hh, "Old starting frequency: " << startFreq);
        startFreq = sqrt( startFreq * startFreq + sep * sep + magnetron * magnetron );
        KTINFO(avlog_hh, "New starting frequency: " << startFreq);
        trackData.SetStartFrequency( startFreq - GetMixingOffset() );

        endFreq = sqrt( endFreq * endFreq + sep * sep + magnetron * magnetron );
        trackData.SetEndFrequency( endFreq - GetMixingOffset() );

        intercept = sqrt( intercept * intercept + sep * sep + magnetron * magnetron );
        trackData.SetIntercept( intercept - GetMixingOffset() );

        return true;
    }

    bool KTSidebandCorrection::AssignFrequencyOffset( KTEggHeader& header )
    {
        double f = 24.2e9 + header.GetMinimumFrequency();
        SetMixingOffset( f );

        KTINFO(avlog_hh, "Set mixing offset to " << GetMixingOffset());

        return true;
    }

} // namespace Katydid
