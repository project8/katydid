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


        return true;
    }

} // namespace Katydid
