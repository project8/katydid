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

    class KTRawTimeSeries :public KTPhysicalArray< 1, uint16_t >
    {
        public:
            KTRawTimeSeries();
            virtual ~KTRawTimeSeries();

    };

} /* namespace Katydid */
#endif /* KTRAWTIMESERIES_HH_ */
