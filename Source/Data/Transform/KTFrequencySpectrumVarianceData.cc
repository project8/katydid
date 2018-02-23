/*
 * KTFrequencySpectrumVarianceData.cc
 *
 *  Created on: Feb 23, 2018
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumVarianceData.hh"

namespace Katydid
{
    KTFrequencySpectrumVarianceDataCore::KTFrequencySpectrumVarianceDataCore() :
            KTFrequencyDomainArrayData()
    {
    }

    KTFrequencySpectrumVarianceDataCore::~KTFrequencySpectrumVarianceDataCore()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }

} /* namespace Katydid */
