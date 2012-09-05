/*
 * KTTimeSeries.hh
 *
 *  Created on: Sept 4, 2012
 *      Author: nsoblath
 */

#ifndef KTTIMESERIES_HH_
#define KTTIMESERIES_HH_

#include "KTPhysicalArray.hh"

#include "Rtypes.h"

#include <string>

#ifdef ROOT_FOUND
class TH1D;
#endif

namespace Katydid
{
    class KTTimeSeries : public KTPhysicalArray< 1, Double_t >
    {
        public:
            KTTimeSeries();
            KTTimeSeries(size_t nBins, Double_t rangeMin=0., Double_t rangeMax=1.);
            KTTimeSeries(const KTTimeSeries& orig);
            virtual ~KTTimeSeries();

            virtual KTTimeSeries& operator=(const KTTimeSeries& rhs);

            void Print(unsigned startPrint, unsigned nToPrint) const;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateHistogram(const std::string& name = "hTimeSeries") const;

            virtual TH1D* CreateAmplitudeDistributionHistogram(const std::string& name = "hTimeSeriesDist") const;
#endif
    };

} /* namespace Katydid */
#endif /* KTTIMESERIES_HH_ */
