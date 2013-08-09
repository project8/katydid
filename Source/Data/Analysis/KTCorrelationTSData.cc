/*
 * KTCorrelationTSData.cc
 *
 *  Created on: Aug 9, 2013
 *      Author: nsoblath
 */

#include "KTCorrelationTSData.hh"

namespace Katydid
{
    KTCorrelationTSData::KTCorrelationTSData() :
            KTTimeSeriesDataCore(),
            KTExtensibleData< KTCorrelationTSData >(),
            fComponentData(1)
    {}
    KTCorrelationTSData::~KTCorrelationTSData()
    {}

    KTCorrelationTSData& KTCorrelationTSData::SetNComponents(UInt_t components)
    {
        UInt_t oldSize = fTimeSeries.size();
        // if components < oldSize
        for (UInt_t iComponent = components; iComponent < oldSize; iComponent++)
        {
            delete fTimeSeries[iComponent];
        }
        fTimeSeries.resize(components);
        fComponentData.resize(components);
        // if components > oldSize
        for (UInt_t iComponent = oldSize; iComponent < components; iComponent++)
        {
            fTimeSeries[iComponent] = NULL;
        }
    return *this;
    }

} /* namespace Katydid */
