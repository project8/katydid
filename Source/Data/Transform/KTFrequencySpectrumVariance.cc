/*
 * KTFrequencySpectrumVariance.cc
 *
 *  Created on: Feb 23, 2018
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumVariance.hh"

namespace Katydid
{
    KTFrequencySpectrumVariance::KTFrequencySpectrumVariance(size_t nBins, double rangeMin, double rangeMax) :
            KTPhysicalArray< 1, double >(nBins, rangeMin, rangeMax),
            KTFrequencyDomainArray()
    {
        SetAxisLabel("Frequency (Hz)");
        SetDataLabel("Arb");
    }

    KTFrequencySpectrumVariance::KTFrequencySpectrumVariance(double value, size_t nBins, double rangeMin, double rangeMax) :
            KTFrequencySpectrumVariance(nBins, rangeMin, rangeMax)
    {
        for (unsigned index = 0; index < nBins; ++index)
        {
            fData[index] = value;
        }
    }

    KTFrequencySpectrumVariance::KTFrequencySpectrumVariance(const KTFrequencySpectrumVariance& orig) :
            KTPhysicalArray< 1, double >(orig),
            KTFrequencyDomainArray(orig)
    {
    }

    KTFrequencySpectrumVariance::~KTFrequencySpectrumVariance()
    {
    }


    KTFrequencySpectrumVariance& KTFrequencySpectrumVariance::operator=(const KTFrequencySpectrumVariance& orig)
    {
        KTPhysicalArray< 1, double >::operator=(orig);
        return *this;
    }

    KTFrequencySpectrumVariance& KTFrequencySpectrumVariance::Scale(double scale)
    {
        (*this) *= scale;
        return *this;
    }

} /* namespace Katydid */
