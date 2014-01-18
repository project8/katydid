/*
 * KTTimeSeries.hh
 *
 *  Created on: Sept 4, 2012
 *      Author: nsoblath
 */

#ifndef KTTIMESERIES_HH_
#define KTTIMESERIES_HH_

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

            /// Scale the time series
            virtual void Scale(double scale) = 0;

            /// Get the size of the time series via the KTTimeSeries interface
            virtual unsigned GetNTimeBins() const = 0;

            /// Get the bin width via the KTTimeSeries interface
            virtual double GetTimeBinWidth() const = 0;

            /// Set values in the time series via the KTTimeSeries interface
            virtual void SetValue(unsigned bin, double value) = 0;

            /// Get values in the time series via the KTTimeSeries interface
            virtual double GetValue(unsigned bin) const = 0;

            virtual void Print(unsigned startPrint, unsigned nToPrint) const = 0;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateHistogram(const std::string& name = "hTimeSeries") const = 0;

            virtual TH1D* CreateAmplitudeDistributionHistogram(const std::string& name = "hTimeSeriesDist") const = 0;
#endif
    };

} /* namespace Katydid */
#endif /* KTTIMESERIES_HH_ */
