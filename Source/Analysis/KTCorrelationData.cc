/*
 * KTCorrelationData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTCorrelationData.hh"

#include "KTDataMap.hh"
#include "KTFrequencySpectrum.hh"
#include "KTTIFactory.hh"
#include "KTWriter.hh"

namespace Katydid
{
    static KTDerivedTIRegistrar< KTDataMap, KTDerivedDataMap< KTCorrelationData > > sCDMRegistrar;

    KTCorrelationData::KTCorrelationData(UInt_t nPairs) :
            KTWriteableData(),
            fData(nPairs),
            fTimeInRun(0.),
            fSliceNumber(0)
    {
    }

    KTCorrelationData::~KTCorrelationData()
    {
        while (! fData.empty())
        {
            delete fData.back().fCorrelation;
            fData.pop_back();
        }
    }

    void KTCorrelationData::Accept(KTWriter* writer) const
    {
        writer->Write(this);
        return;
    }


} /* namespace Katydid */

