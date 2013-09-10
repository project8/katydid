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

    KTFrequencySpectrumDataPolar& KTFrequencySpectrumDataPolar::SetNComponents(UInt_t components)
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

