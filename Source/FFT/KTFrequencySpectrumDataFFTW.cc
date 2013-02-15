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
    KTFrequencySpectrumDataFFTW::KTFrequencySpectrumDataFFTW(UInt_t nChannels) :
            KTData< KTFrequencySpectrumDataFFTW >(),
            fSpectra(nChannels)
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

