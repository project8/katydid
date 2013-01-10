#ifndef __KT_BACM_HH
#define __KT_BACM_HH
/*
 *  KTBiasedACM.hh
 *    author: kofron
 *    created: 12/30/2012
 *  KTBiasedACM is an abstract representation of a biased autocorrelation 
 *  matrix (ACM) R.  R_ij = R_xx(i-j) = sum_l x(l)x(l + (i-j)).
 */

#include <Eigen/Dense>

namespace Katydid {

  typedef Eigen::MatrixXd KTBiasedACM;

}; // namespace katydid

#endif // __KT_BACM_HH
