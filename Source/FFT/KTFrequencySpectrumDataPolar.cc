/*
 * KTFrequencySpectrumDataPolar.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumDataPolar.hh"

#include "KTFrequencySpectrumPolar.hh"

namespace Katydid
{
    KTFrequencySpectrumDataPolar::KTFrequencySpectrumDataPolar(UInt_t nChannels) :
            KTData< KTFrequencySpectrumDataPolar >(),
            fSpectra(nChannels)
    {
    }

    KTFrequencySpectrumDataPolar::~KTFrequencySpectrumDataPolar()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }

} /* namespace Katydid */

