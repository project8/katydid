/*
 * KTTrackProcessing.cc
 *
 *  Created on: July 22, 2013
 *      Author: N.S. Oblath & B. LaRoque
 */

#include "KTTrackProcessing.hh"

#include "KTHoughData.hh"
#include "KTLogger.hh"
#include "KTNOFactory.hh"
#include "KTParam.hh"
#include "KTSparseWaterfallCandidateData.hh"

using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(tlog, "katydid.fft");

    // Register the processor
    KT_REGISTER_PROCESSOR(KTTrackProcessing, "track-proc");

    KTTrackProcessing::KTTrackProcessing(const std::string& name) :
            KTProcessor(name),
            fTrackSignal("track", this),
            fSWFAndHoughSlot("swfc-and-hough", this, &KTTrackProcessing::ProcessTrack, &fTrackSignal)
    {
    }

    KTTrackProcessing::~KTTrackProcessing()
    {
    }

    bool KTTrackProcessing::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        return true;
    }

    bool KTTrackProcessing::ProcessTrack(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData)
    {
        return true;
    }


} /* namespace Katydid */
