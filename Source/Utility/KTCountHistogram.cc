/*
 * KTCountHistogram.cc
 *
 *  Created on: Dec 20, 2013
 *      Author: nsoblath
 */

#include "KTCountHistogram.hh"

namespace Katydid
{

    KTCountHistogram::KTCountHistogram() :
            KTPhysicalArray< 1, unsigned >()
    {
        fData[0] = 0.;
    }

    KTCountHistogram::KTCountHistogram(size_t nBins, double rangeMin, double rangeMax) :
            KTPhysicalArray< 1, unsigned >(nBins, rangeMin, rangeMax)
    {
        for (unsigned iBin = 0; iBin < nBins; ++iBin)
        {
            fData[iBin] = 0.0;
        }
    }

    KTCountHistogram::KTCountHistogram(const KTCountHistogram& orig) :
            KTPhysicalArray< 1, unsigned >(orig)
    {
    }

    KTCountHistogram::~KTCountHistogram()
    {
    }

    KTCountHistogram& KTCountHistogram::operator=(const KTCountHistogram& rhs)
    {
        KTPhysicalArray< 1, unsigned >::operator=(rhs);
        return *this;
    }

    void KTCountHistogram::Increment(unsigned bin, int delta)
    {
        (*this)(bin) = (*this)(bin) + delta;
    }

    void KTCountHistogram::Increment(double value, int delta)
    {
        return Increment((unsigned)FindBin(value), delta);
    }


} /* namespace Katydid */
