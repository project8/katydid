#ifndef __KTRQ_PROC_HH
#define __KTRQ_PROC_HH

/*
 *  KTRQProcessor.hh
 *   author: kofron
 *   created: 12/30/2012
 *
 *  KTRQProcessor calculates Rayleigh Quotients for time series data.
 *  It requires two time series inputs - one noise to generate the noise
 *  ACM, and another "interesting" input which is the data over which the
 *  rayleigh quotient is calculated.
 */

#include "KTBiasedACM.hh"
#include "KTProcessor.hh"

namespace Katydid {

  class KTRQProcessor : public KTProcessor {

  }; // class KTRQProcessor

}; // namespace katydid

#endif // __KTRQ_PROC_HH
