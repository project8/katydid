/*
 * KTWignerVille.cc
 *
 *  Created on: Oct 19, 2012
 *      Author: nsoblath
 */

#include "KTWignerVilleData.hh"


namespace Katydid
{
    KTWignerVilleData::KTWignerVilleData() :
            KTFrequencySpectrumDataFFTWCore(),
            KTExtensibleData< KTWignerVilleData >(),
            fWVComponentData(1)
    {}
    KTWignerVilleData::~KTWignerVilleData()
    {}

    KTWignerVilleData& KTWignerVilleData::SetNComponents(unsigned components)
    {
        unsigned oldSize = fSpectra.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize(components);
        fWVComponentData.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
        return *this;
    }
} /* namespace Katydid */
