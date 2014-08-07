/*
 * KTWignerVille.cc
 *
 *  Created on: May 16, 2013
 *      Author: nsoblath
 */

#include "KTWV2DData.hh"


namespace Katydid
{
    KTWV2DData::KTWV2DData() :
            KTMultiFSDataFFTWCore(),
            KTExtensibleData< KTWV2DData >(),
            fWVComponentData(1)
    {
    }

    KTWV2DData::~KTWV2DData()
    {
    }


    KTWV2DData& KTWV2DData::SetNComponents(unsigned components)
    {
        unsigned oldSize = fSpectra.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
        {
            DeleteSpectra(iComponent);
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
