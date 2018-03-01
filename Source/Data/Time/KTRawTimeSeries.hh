/*
 * KTRawTimeSeries.hh
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 *
 *  NOTE: For complex sampling, KTRawTimeSeries consists of a single array with interleaved real and imaginary samples.
 *        A KTRawTimeSeries with N complex samples will have 2*N bins.
 */

#ifndef KTRAWTIMESERIES_HH_
#define KTRAWTIMESERIES_HH_

#include "KTVarTypePhysicalArray.hh"

#include "KTMemberVariable.hh"

namespace Katydid
{
    

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

            MEMBERVARIABLE(size_t, SampleSize);

    };

    template< typename XInterfaceType >
    KTVarTypePhysicalArray< XInterfaceType > KTRawTimeSeries::CreateInterface() const
    {
        return KTVarTypePhysicalArray< XInterfaceType >( *this, false );
    }


} /* namespace Katydid */
#endif /* KTRAWTIMESERIES_HH_ */
