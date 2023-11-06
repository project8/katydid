/*
 * KTInnerProductData.hh
 *
 *  Created on: Apr 28, 2021
 *      Author: F. Thomas
 */

#ifndef KTINNERPRODUCTDATA_HH_
#define KTINNERPRODUCTDATA_HH_

#include "KTData.hh"
#include "KTPhysicalArrayComplex.hh"


namespace Katydid
{
    
    class KTInnerProductData: public KTPhysicalArray<2, std::complex<double>>, public Nymph::KTExtensibleData< KTInnerProductData >
    {
        public:

            static const std::string sName;

    };

} /* namespace Katydid */

#endif /* KTINNERPRODUCTDATA_HH_ */
