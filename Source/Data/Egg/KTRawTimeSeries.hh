/*
 * KTRawTimeSeries.hh
 *
 *  Created on: Dec 23, 2013
 *      Author: nsoblath
 */

#ifndef KTRAWTIMESERIES_HH_
#define KTRAWTIMESERIES_HH_

#include "KTPhysicalArray.hh"

#include <stdint.h>

namespace Katydid
{

    class KTRawTimeSeries :public KTPhysicalArray< 1, uint64_t >
    {
        public:
            KTRawTimeSeries();
            KTRawTimeSeries(size_t nBins, double rangeMin, double rangeMax);
            KTRawTimeSeries(const KTRawTimeSeries& orig);
            virtual ~KTRawTimeSeries();

            KTRawTimeSeries& operator=(const KTRawTimeSeries& rhs);

    };

} /* namespace Katydid */
#endif /* KTRAWTIMESERIES_HH_ */
