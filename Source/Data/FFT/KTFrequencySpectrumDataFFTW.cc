/*
 * KTFrequencySpectrumDataFFTW.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTFrequencySpectrumDataFFTW.hh"

namespace Katydid
{
    KTFrequencySpectrumDataFFTWCore::KTFrequencySpectrumDataFFTWCore() :
            fSpectra(1)
    {
        fSpectra[0] = NULL;
    }

    KTFrequencySpectrumDataFFTWCore::~KTFrequencySpectrumDataFFTWCore()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }


    KTFrequencySpectrumDataFFTW::KTFrequencySpectrumDataFFTW() :
            KTFrequencySpectrumDataFFTWCore(),
            KTExtensibleData< KTFrequencySpectrumDataFFTW >()
    {
    }

    KTFrequencySpectrumDataFFTW::~KTFrequencySpectrumDataFFTW()
    {
    }

    KTFrequencySpectrumDataFFTW& KTFrequencySpectrumDataFFTW::SetNComponents(UInt_t components)
    {
        UInt_t oldSize = fSpectra.size();
        // if components < oldSize
        for (UInt_t iComponent = components; iComponent < oldSize; iComponent++)
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize(components);
        // if components > oldSize
        for (UInt_t iComponent = oldSize; iComponent < components; iComponent++)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }



} /* namespace Katydid */

