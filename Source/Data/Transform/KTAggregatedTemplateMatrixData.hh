/*
 * KTAggregatedTemplateMatrixData.hh
 *
 *  Created on: May 4, 2021
 *      Author: F. Thomas
 */

#ifndef KTAGGREGATEDTEMPLATEMATRIXDATA_HH_
#define KTAGGREGATEDTEMPLATEMATRIXDATA_HH_

#include "KTData.hh"
#include "KTPhysicalArrayComplex.hh"

namespace Katydid
{
    
    class KTAggregatedTemplateMatrixData: public KTPhysicalArray<2, std::complex<double>>, public Nymph::KTExtensibleData< KTAggregatedTemplateMatrixData >
    {
        public:
    	/*
            KTAggregatedTemplateMatrixData() = default;

            virtual ~KTAggregatedTemplateMatrixData() = default;
            KTAggregatedTemplateMatrixData (KTAggregatedTemplateMatrixData &&) = default;
            KTAggregatedTemplateMatrixData ( const KTAggregatedTemplateMatrixData &) = default;

            KTAggregatedTemplateMatrixData& operator=( const KTAggregatedTemplateMatrixData &) = default;
            KTAggregatedTemplateMatrixData& operator=( KTAggregatedTemplateMatrixData && ) = default;
		*/

            static const std::string sName;

    };

} /* namespace Katydid */

#endif /* KTAGGREGATEDTEMPLATEMATRIXDATA_HH_ */
