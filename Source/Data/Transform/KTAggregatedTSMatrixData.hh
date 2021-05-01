/*
 * KTAggregatedTSMatrixData.hh
 *
 *  Created on: Apr 29, 2021
 *      Author: F. Thomas
 */

#ifndef KTAGGREGATEDTSMATRIXDATA_HH_
#define KTAGGREGATEDTSMATRIXDATA_HH_

#include "KTData.hh"
#include "KTPhysicalArrayComplex.hh"

namespace Katydid
{
    
    class KTAggregatedTSMatrixData: public KTPhysicalArray<2, std::complex<double>>, public Nymph::KTExtensibleData< KTAggregatedTSMatrixData >
    {
        public:
            KTAggregatedTSMatrixData() = default;

            virtual ~KTAggregatedTSMatrixData() = default;
            KTAggregatedTSMatrixData (KTAggregatedTSMatrixData &&) = default;
            KTAggregatedTSMatrixData ( const KTAggregatedTSMatrixData &) = default;

            KTAggregatedTSMatrixData& operator=( const KTAggregatedTSMatrixData &) = default;
            KTAggregatedTSMatrixData& operator=( KTAggregatedTSMatrixData && ) = default;

            static const std::string sName;

    };

} /* namespace Katydid */

#endif /* KTAGGREGATEDTSMATRIXDATA_HH_ */
