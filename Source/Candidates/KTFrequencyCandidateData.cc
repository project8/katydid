/*
 * KTFrequencyCandidateData.cc
 *
 *  Created on: Dec 18, 2012
 *      Author: nsoblath
 */

#include "KTFrequencyCandidateData.hh"

#include "KTDataMap.hh"
#include "KTTIFactory.hh"

namespace Katydid
{

    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTFrequencyCandidateData > > sFCDMRegistrar;

    KTFrequencyCandidateData::KTFrequencyCandidateData()
    {
    }

    KTFrequencyCandidateData::~KTFrequencyCandidateData()
    {
    }

    void KTFrequencyCandidateData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }


} /* namespace Katydid */
