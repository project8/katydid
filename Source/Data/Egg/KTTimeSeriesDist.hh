/*
 * KTTimeSeriesDist.hh
 *
 *  Created on: Jan 21, 2014
 *      Author: laroque
 */

#ifndef KTTIMESERIESDIST_HH_
#define KTTIMESERIESDIST_HH_

#include <string>

#ifdef ROOT_FOUND
class TH1D;
#endif

namespace Katydid
{
    class KTTimeSeriesDist
    {
        public:
            KTTimeSeriesDist();
            virtual ~KTTimeSeriesDist();

            /// Scale the time series
            virtual void Scale(double scale) = 0;

            /// Get the size of the time series via the KTTimeSeriesDist interface
            virtual unsigned GetNTimeBins() const = 0;

            /// Get the bin width via the KTTimeSeriesDist interface
            virtual double GetTimeBinWidth() const = 0;

            /// Set values in the time series via the KTTimeSeriesDist interface
            virtual void SetValue(unsigned bin, double value) = 0;

            /// Get values in the time series via the KTTimeSeriesDist interface
            virtual double GetValue(unsigned bin) const = 0;

            virtual void Print(unsigned startPrint, unsigned nToPrint) const = 0;

#ifdef ROOT_FOUND
        public:
            virtual TH1D* CreateHistogram(const std::string& name = "hTimeSeriesDist") const = 0;

            virtual TH1D* CreateAmplitudeDistributionHistogram(const std::string& name = "hTimeSeriesDist") const = 0;
#endif
    };

} /* namespace Katydid */
#endif /* KTTIMESERIESDIST_HH_ */
