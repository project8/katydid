/*
 * KTCorrelationData.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTCorrelationData.hh"

#include "KTFrequencySpectrumPolar.hh"

namespace Katydid
{
    KTCorrelationData::KTCorrelationData() :
            KTData< KTCorrelationData >(),
            fData(1)
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

} /* namespace Katydid */

