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
            fSpectra(1)
    {
        fSpectra[0] = NULL;
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

