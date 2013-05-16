/*
 * KTWignerVille.cc
 *
 *  Created on: May 16, 2013
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

    KTWignerVilleData& KTWignerVilleData::SetNComponents(UInt_t components)
    {
        UInt_t oldSize = fSpectra.size();
        fSpectra.resize(components);
        fWVComponentData.resize(components);
        if (components > oldSize)
        {
            for (UInt_t iComponent = oldSize; iComponent < components; iComponent++)
            {
                fSpectra[iComponent] = NULL;
            }
        }
        return *this;
    }
} /* namespace Katydid */
