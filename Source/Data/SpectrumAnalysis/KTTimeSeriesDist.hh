/*
 * KTTimeSeriesDist.hh
 *
 *  Created on: Jan 22, 2014
 *      Author: laroque
 */

#ifndef KTTIMESERIESDIST_HH_
#define KTTIMESERIESDIST_HH_

#include "KTPhysicalArray.hh"
#include "KTRawTimeSeries.hh"

#include <stdint.h>


namespace Katydid
{
    using namespace Nymph;

    class KTTimeSeriesDist :public KTPhysicalArray< 1, double >
    {
        public:
            KTTimeSeriesDist();
            KTTimeSeriesDist(size_t nBins, double rangeMin, double rangeMax);
            KTTimeSeriesDist(const KTTimeSeriesDist& orig);
            virtual ~KTTimeSeriesDist();

            KTTimeSeriesDist& operator=(const KTTimeSeriesDist& rhs);

            KTTimeSeriesDist& Scale(double scale);

    };

} /* namespace Katydid */
#endif /* KTTIMESERIESDIST_HH_ */
