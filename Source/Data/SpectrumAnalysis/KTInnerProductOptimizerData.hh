/*
 * KTInnerProductOptimizerData.hh
 *
 *  Created on: May 5, 2021
 *      Author: F. Thomas
 */

#ifndef KTINNERPRODUCTOPTIMIZERDATA_HH_
#define KTINNERPRODUCTOPTIMIZERDATA_HH_

#include "KTData.hh"
#include "KTPhysicalArrayComplex.hh"

#include <eigen3/Eigen/Dense>


namespace Katydid
{
    
    class KTInnerProductOptimizerData: public Nymph::KTExtensibleData< KTInnerProductOptimizerData >
    {
        public:

            static const std::string sName;

            Eigen::ArrayXd fMaxVals;
            Eigen::Array< Eigen::MatrixXf::Index, Eigen::Dynamic, 1> fMaxInds;

    };

} /* namespace Katydid */

#endif /* KTINNERPRODUCTOPTIMIZERDATA_HH_ */
