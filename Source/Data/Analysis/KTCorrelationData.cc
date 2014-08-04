/*
 * KTCorrelationData.cc
 *
 *  Created on: Aug 20, 2012
 *      Author: nsoblath
 */

#include "KTCorrelationData.hh"

namespace Katydid
{
    KTCorrelationData::KTCorrelationData() :
            KTFrequencySpectrumDataPolarCore(),
            KTExtensibleData< KTCorrelationData >(),
            fComponentData(1)
    {}
    KTCorrelationData::~KTCorrelationData()
    {}

    KTCorrelationData& KTCorrelationData::SetNComponents(unsigned components)
    {
        unsigned oldSize = fSpectra.size();
        // if components < oldSize
        for (unsigned iComponent = components; iComponent < oldSize; ++iComponent)
        {
            delete fSpectra[iComponent];
        }
        fSpectra.resize(components);
        fComponentData.resize(components);
        // if components > oldSize
        for (unsigned iComponent = oldSize; iComponent < components; ++iComponent)
        {
            fSpectra[iComponent] = NULL;
        }
    return *this;
    }

} /* namespace Katydid */
