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
            KTPhysicalArray< 1, double >()
    {
        fData[0] = 0;
    }

    KTTimeSeriesDist::KTTimeSeriesDist(size_t nBins) :
            KTPhysicalArray< 1, double >(nBins)
    {
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            fData[iBin] = 0;
        }
    }
    KTTimeSeriesDist::KTTimeSeriesDist(const KTTimeSeriesDist& orig) :
            KTPhysicalArray< 1, double >(orig)
    {
    }

    KTTimeSeriesDist::~KTTimeSeriesDist()
    {
    }

    KTTimeSeriesDist& KTTimeSeriesDist::operator=(const KTTimeSeriesDist& rhs)
    {
        KTPhysicalArray< 1, double >::operator=(rhs);
        return *this;
    }

} /* namespace Katydid */
