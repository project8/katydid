/*
 * KTFrequencySpectrumDataFFTW.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumDataFFTW.hh"

#include "KTFrequencySpectrumFFTW.hh"

namespace Katydid
{
    KTFrequencySpectrumDataFFTW::KTFrequencySpectrumDataFFTW() :
            KTExtensibleData< KTFrequencySpectrumDataFFTW >(),
            fSpectra(1)
    {
    }

    KTFrequencySpectrumDataFFTW::~KTFrequencySpectrumDataFFTW()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }

} /* namespace Katydid */

