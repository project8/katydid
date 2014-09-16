/*
 * KTWaterfallCandidateData.cc
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#include "KTWaterfallCandidateData.hh"

namespace Katydid
{
    const std::string KTWaterfallCandidateData::sName("waterfall-candidate");

    KTWaterfallCandidateData::KTWaterfallCandidateData() :
            KTExtensibleData< KTWaterfallCandidateData >(),
            fCandidate(NULL),
            fComponent(0),
            fTimeInRun(0.),
            fTimeLength(0.),
            fFirstSliceNumber(0),
            fLastSliceNumber(0),
            fMinFrequency(0.),
            fMaxFrequency(0.),
            fMeanStartFrequency(0.),
            fMeanEndFrequency(0.),
            fFrequencyWidth(0.),
            fStartRecordNumber(0),
            fStartSampleNumber(0),
            fEndRecordNumber(0),
            fEndSampleNumber(0)
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
