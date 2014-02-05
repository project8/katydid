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

    class KTTimeSeriesDist :public KTPhysicalArray< 1, uint32_t >
    {
        public:
            KTTimeSeriesDist();
            KTTimeSeriesDist(size_t nBins, double rangeMin, double rangeMax);
            KTTimeSeriesDist(const KTTimeSeriesDist& orig);
            virtual ~KTTimeSeriesDist();

            KTTimeSeriesDist& operator=(const KTTimeSeriesDist& rhs);
#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateHistogram(const std::string& name = "hTimeSeriesDist") const = 0;
#endif

    };

} /* namespace Katydid */
#endif /* KTTIMESERIESDIST_HH_ */
