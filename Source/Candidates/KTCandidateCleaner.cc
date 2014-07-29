/*
 * KTCandidateCleaner.cc
 *
 *  Created on: July 22, 2013
 *      Author: N.S. Oblath & B. LaRoque
 */

#include "KTCandidateCleaner.hh"

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
    KT_REGISTER_PROCESSOR(KTCandidateCleaner, "candidate-cleaner");

    KTCandidateCleaner::KTCandidateCleaner(const std::string& name) :
            KTProcessor(name),
            fCleanedCandidateSignal("clean-cand", this),
            fSWFCAndHoughSlot("swfc-and-hough", this, &KTCandidateCleaner::CleanCandidate, &fCleanedCandidateSignal)
    {
    }

    KTCandidateCleaner::~KTCandidateCleaner()
    {
    }

    bool KTCandidateCleaner::Configure(const KTParamNode* node)
    {
        if (node == NULL) return false;

        return true;
    }

    bool KTCandidateCleaner::CleanCandidate(KTSparseWaterfallCandidateData& swfData, KTHoughData& htData)
    {
        return true;
    }


} /* namespace Katydid */
