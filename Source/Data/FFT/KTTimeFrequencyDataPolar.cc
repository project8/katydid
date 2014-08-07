/*
 * KTTimeFrequencyDataPolar.cc
 *
 *  Created on: May 16, 2013
 *      Author: nsoblath
 */

#include "KTTimeFrequencyDataPolar.hh"

namespace Katydid
{
    KTTimeFrequencyDataPolarCore::KTTimeFrequencyDataPolarCore() :
            fSpectra(1)
    {
        fSpectra[0] = NULL;
    }

    KTTimeFrequencyDataPolarCore::~KTTimeFrequencyDataPolarCore()
    {
        while (! fSpectra.empty())
        {
            delete fSpectra.back();
            fSpectra.pop_back();
        }
    }


    KTTimeFrequencyDataPolar::KTTimeFrequencyDataPolar() :
            KTTimeFrequencyDataPolarCore(),
            KTExtensibleData< KTTimeFrequencyDataPolar >()
    {
    }

    KTTimeFrequencyDataPolar::~KTTimeFrequencyDataPolar()
    {
    }

    KTTimeFrequencyDataPolar& KTTimeFrequencyDataPolar::SetNComponents(unsigned components)
    {
        unsigned oldSize = fSpectra.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }

} /* namespace Katydid */

