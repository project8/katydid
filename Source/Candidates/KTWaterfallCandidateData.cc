/*
 * KTWaterfallCandidateData.cc
 *
 *  Created on: Feb 7, 2013
 *      Author: nsoblath
 */

#include "KTWaterfallCandidateData.hh"

#include "KTDataMap.hh"
#include "KTTimeFrequency.hh"
#include "KTTIFactory.hh"
#include "KTWriter.hh"

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTWaterfallCandidateData > > sWCDMRegistrar;

    KTWaterfallCandidateData::KTWaterfallCandidateData() :
            KTWriteableData(),
            fCandidate(NULL),
            fComponent(0),
            fTimeInRun(-1.),
            fTimeLength(-1.),
            fFirstSliceNumber(0),
            fLastSliceNumber(0)
    {
    }

    KTWaterfallCandidateData::~KTWaterfallCandidateData()
    {
        delete fCandidate;
    }

    void KTWaterfallCandidateData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }

    void KTWaterfallCandidateData::SetCandidate(KTTimeFrequency* candidate)
    {
        delete fCandidate;
        fCandidate = candidate;
        return;
    }




} /* namespace Katydid */
