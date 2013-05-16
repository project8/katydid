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
            //KTTimeFrequencyDataFFTWCore(),
            KTTimeFrequencyDataPolarCore(),
            KTExtensibleData< KTWV2DData >(),
            fWVComponentData(1)
    {}
    KTWV2DData::~KTWV2DData()
    {}

    KTWV2DData& KTWV2DData::SetNComponents(UInt_t components)
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
