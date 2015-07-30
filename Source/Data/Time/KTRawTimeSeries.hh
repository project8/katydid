/*
 * KTRawTimeSeries.hh
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#ifndef KTRAWTIMESERIES_HH_
#define KTRAWTIMESERIES_HH_

#include "KTVarTypePhysicalArray.hh"

namespace Katydid
{
    using namespace Nymph;

    class KTRawTimeSeries : public KTVarTypePhysicalArray< uint64_t >
    {
        public:
            KTRawTimeSeries();
            KTRawTimeSeries(size_t dataTypeSize, uint32_t dataFormat, size_t nBins, double rangeMin, double rangeMax);
            KTRawTimeSeries(const KTRawTimeSeries& orig);
            virtual ~KTRawTimeSeries();

            KTRawTimeSeries& operator=(const KTRawTimeSeries& rhs);

            /// Create an interface object with a different interface type
            template< typename XInterfaceType >
            KTVarTypePhysicalArray< XInterfaceType > CreateInterface() const;

    };

    template< typename XInterfaceType >
    KTVarTypePhysicalArray< XInterfaceType > KTRawTimeSeries::CreateInterface() const
    {
        return KTVarTypePhysicalArray< XInterfaceType >( *this, false );
    }


} /* namespace Katydid */
#endif /* KTRAWTIMESERIES_HH_ */
