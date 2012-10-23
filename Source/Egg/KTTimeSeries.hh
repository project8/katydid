/*
 * KTTimeSeries.hh
 *
 *  Created on: Sept 4, 2012
 *      Author: nsoblath
 */

#ifndef KTTIMESERIES_HH_
#define KTTIMESERIES_HH_

#include "Rtypes.h"

#include <string>

#ifdef ROOT_FOUND
class TH1D;
#endif

namespace Katydid
{
    class KTTimeSeries
    {
        public:
            KTTimeSeries();
            virtual ~KTTimeSeries();

            /// Get the size of the time series via the KTTimeSeries interface
            virtual UInt_t GetNTimeBins() const = 0;

            /// Get the bin width via the KTTimeSeries interface
            virtual Double_t GetTimeBinWidth() const = 0;

            /// Set values in the time series via the KTTimeSeries interface
            virtual void SetValue(UInt_t bin, Double_t value) = 0;

            /// Get values in the time series via the KTTimeSeries interface
            virtual Double_t GetValue(UInt_t bin) const = 0;

            virtual void Print(UInt_t startPrint, UInt_t nToPrint) const = 0;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateHistogram(const std::string& name = "hTimeSeries") const = 0;

            virtual TH1D* CreateAmplitudeDistributionHistogram(const std::string& name = "hTimeSeriesDist") const = 0;
#endif
    };

} /* namespace Katydid */
#endif /* KTTIMESERIES_HH_ */
