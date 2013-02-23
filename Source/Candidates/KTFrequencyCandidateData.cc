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

    KTFrequencyCandidateData::KTFrequencyCandidateData(UInt_t nComponents) :
            KTWriteableData(),
            fComponentData(nComponents),
            fNBins(1),
            fBinWidth(1.),
            fTimeInRun(0.),
            fSliceNumber(0)
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
