/*
 * KTTimeSeriesDist.cc
 *
 *  Created on: Jan 22, 2014
 *      Author: laroque
 */

#include "KTTimeSeriesDist.hh"

namespace Katydid
{

    KTTimeSeriesDist::KTTimeSeriesDist() :
            KTPhysicalArray< 1, uint32_t >()
    {
        fData[0] = 0;
    }

    KTTimeSeriesDist::KTTimeSeriesDist(size_t nBins, double rangeMin, double rangeMax) :
            KTPhysicalArray< 1, uint32_t >(nBins, rangeMin, rangeMax)
    {
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            fData[iBin] = 0;
        }
    }
    KTTimeSeriesDist::KTTimeSeriesDist(const KTTimeSeriesDist& orig) :
            KTPhysicalArray< 1, uint32_t >(orig)
    {
    }

    KTTimeSeriesDist::~KTTimeSeriesDist()
    {
    }

    KTTimeSeriesDist& KTTimeSeriesDist::operator=(const KTTimeSeriesDist& rhs)
    {
        KTPhysicalArray< 1, uint32_t >::operator=(rhs);
        return *this;
    }

} /* namespace Katydid */
