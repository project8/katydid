/*
 * KTFrequencyCandidateData.cc
 *
 *  Created on: Dec 18, 2012
 *      Author: nsoblath
 */

#include "KTFrequencyCandidateData.hh"

namespace Katydid
{

    KTFrequencyCandidateData::KTFrequencyCandidateData(UInt_t nGroups) :
            KTWriteableData(),
            fGroupData(nGroups),
            fNBins(1),
            fBinWidth(1.)
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
