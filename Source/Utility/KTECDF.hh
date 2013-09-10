#ifndef __KTECDF__HH
#define __KTECDF__HH

/*
 *  KTECDF.hh
 *     author: kofron
 *     created: 12/30/2012
 *  KTECDF provides an empirical cumulative distribution function which can
 *  be used for hypothesis testing.
 */

// We use the BOOST accumulator framework as the backbone of the ecdf.
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/moment.hpp>
using namespace boost::accumulators;

namespace katydid {

  template <typename T>
  class KTECDF {
  private:
    accumulator_set<T, stats<tag::mean, tag::moment<2> > > fData;
  }; // class KTECDF

}; // namespace katydid

#endif // __KTECDF__HH
