/*
 * KTFrequencyCandidateData.cc
 *
 *  Created on: Dec 18, 2012
 *      Author: nsoblath
 */

#include "KTFrequencyCandidateData.hh"

namespace Katydid
{
    const std::string KTFrequencyCandidateData::sName("frequency-candidate");

    KTFrequencyCandidateData::KTFrequencyCandidateData() :
            KTExtensibleData< KTFrequencyCandidateData >(),
            fComponentData(1),
            fNBins(1),
            fBinWidth(1.)
    {
    }

    KTFrequencyCandidateData::~KTFrequencyCandidateData()
    {
    }

} /* namespace Katydid */
