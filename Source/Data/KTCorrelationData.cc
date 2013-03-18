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

    KTCorrelationData& KTCorrelationData::SetNComponents(UInt_t components)
    {
        UInt_t oldSize = fSpectra.size();
        fSpectra.resize(components);
        fComponentData.resize(components);
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
