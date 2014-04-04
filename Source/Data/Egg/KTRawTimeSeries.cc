/*
 * KTRawTimeSeries.cc
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#include "KTRawTimeSeries.hh"

namespace Katydid
{

    KTRawTimeSeries::KTRawTimeSeries() :
            KTPhysicalArray< 1, uint64_t >()
    {
        fData[0] = 0;
    }

    KTRawTimeSeries::KTRawTimeSeries(size_t nBins, double rangeMin, double rangeMax) :
            KTPhysicalArray< 1, uint64_t >(nBins, rangeMin, rangeMax)
    {
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            fData[iBin] = 0;
        }
    }
    KTRawTimeSeries::KTRawTimeSeries(const KTRawTimeSeries& orig) :
            KTPhysicalArray< 1, uint64_t >(orig)
    {
    }

    KTRawTimeSeries::~KTRawTimeSeries()
    {
    }

    KTRawTimeSeries& KTRawTimeSeries::operator=(const KTRawTimeSeries& rhs)
    {
        KTPhysicalArray< 1, uint64_t >::operator=(rhs);
        return *this;
    }

} /* namespace Katydid */
