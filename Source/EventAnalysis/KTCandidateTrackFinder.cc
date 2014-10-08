/*
 * KTCandidateTrackFinder.cc
 *
 *  Created on: July 22, 2013
 *      Author: N.S. Oblath & B. LaRoque
 */

#include "KTCandidateTrackFinder.hh"

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
    KT_REGISTER_PROCESSOR(KTCandidateTrackFinder, "candidate-track-finder");

    KTCandidateTrackFinder::KTCandidateTrackFinder(const std::string& name) :
            KTProcessor(name),
            fTrackSignal("cand-track", this),
            fSWFCAndHoughSlot("swfc-and-hough", this, &KTCandidateTrackFinder::FindTrack, &fTrackSignal)
    {
    }

    KTCandidateTrackFinder::~KTCandidateTrackFinder()
    {
    }

    bool KTCandidateTrackFinder::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        return true;
    }

    bool KTCandidateTrackFinder::FindTrack(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData)
    {
        return true;
    }


} /* namespace Katydid */
