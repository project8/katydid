/*
 * KTFrequencyCandidateData.cc
 *
 *  Created on: Dec 18, 2012
 *      Author: nsoblath
 */

#include "KTFrequencyCandidateData.hh"

#include "KTDataMap.hh"
#include "KTTIFactory.hh"
#include "KTWriter.hh"

namespace Katydid
{

    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTFrequencyCandidateData > > sFCDMRegistrar;

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
