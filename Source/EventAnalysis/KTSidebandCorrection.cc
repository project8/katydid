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
            fTrackSignal("track", this)
    {
        RegisterSlot( "fit-result", this, &KTSidebandCorrection::SlotFunctionFitResult );
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
        double startFreq = trackData.GetStartFrequency();
        double endFreq = trackData.GetEndFrequency();
        double intercept = trackData.GetIntercept();

        double sidebandIntercept = fitData.GetIntercept( 0 );
        double signalIntercept = fitData.GetIntercept( 1 );
        double sep = abs( signalIntercept - sidebandIntercept );
        double magnetron = fitData.GetFFT_peak( 0 );

        if( sep < 2e6 )
        {
            KTINFO(avlog_hh, "Could not distinguish sideband and signal peaks. Cutting track");
            trackData.SetIsCut( true );

            return true;
        }

        KTINFO(avlog_hh, "Found an axial frequency of " << sep << " Hz. Correcting track frequency data");

        KTINFO(avlog_hh, "Old starting frequency: " << startFreq);
        startFreq = sqrt( startFreq * startFreq + sep * sep );
        KTINFO(avlog_hh, "New starting frequency: " << startFreq);
        trackData.SetStartFrequency( startFreq );

        endFreq = sqrt( endFreq * endFreq + sep * sep );
        trackData.SetEndFrequency( endFreq );

        intercept = sqrt( intercept * intercept + sep * sep );
        trackData.SetIntercept( intercept );

        return true;
    }

} // namespace Katydid
