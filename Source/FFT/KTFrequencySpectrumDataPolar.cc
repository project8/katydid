/*
 * KTFrequencySpectrumDataPolar.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumDataPolar.hh"

namespace Katydid
{
    KTFrequencySpectrumDataPolarCore::KTFrequencySpectrumDataPolarCore() :
            fSpectra()
    {
    }

    KTFrequencySpectrumDataPolarCore::~KTFrequencySpectrumDataPolarCore()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }


    KTFrequencySpectrumDataPolar::KTFrequencySpectrumDataPolar() :
            KTFrequencySpectrumDataPolarCore(),
            KTExtensibleData< KTFrequencySpectrumDataPolar >()
    {
    }

    KTFrequencySpectrumDataPolar::~KTFrequencySpectrumDataPolar()
    {
    }

} /* namespace Katydid */

