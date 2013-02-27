/*
 * KTWaterfallCandidateData.cc
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#include "KTWaterfallCandidateData.hh"

namespace Katydid
{
    KTWaterfallCandidateData::KTWaterfallCandidateData() :
            KTExtensibleData< KTWaterfallCandidateData >(),
            fCandidate(NULL),
            fComponent(0),
            fTimeInRun(0.),
            fTimeLength(0.),
            fFrequencyWidth(0.),
            fFirstSliceNumber(0),
            fLastSliceNumber(0)
    {
    }

    KTWaterfallCandidateData::~KTWaterfallCandidateData()
    {
        delete fCandidate;
    }

    void KTWaterfallCandidateData::SetCandidate(KTTimeFrequency* candidate)
    {
        delete fCandidate;
        fCandidate = candidate;
        return;
    }

} /* namespace Katydid */
