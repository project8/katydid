/*
 * KTCorrelationData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTCorrelationData.hh"

#include "KTFrequencySpectrum.hh"
#include "KTPublisher.hh"

#include <typeinfo>

namespace Katydid
{
    std::string KTCorrelationData::fName("correlation");

    const std::string& KTCorrelationData::StaticGetName()
    {
        return fName;
    }

    KTCorrelationData::KTCorrelationData(UInt_t nPairs) :
            KTWriteableData(),
            fData(nPairs)
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

    void KTCorrelationData::Accept(KTPublisher* publisher) const
    {
        publisher->Write(this);
        return;
    }


} /* namespace Katydid */

