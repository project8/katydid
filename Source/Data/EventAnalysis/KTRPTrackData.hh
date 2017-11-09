/*
 * KTRPTrackData.hh
 *
 *  Created on: Nov 9, 2017
 *      Author: ezayas
 */

#ifndef KTRPTRACKDATA_HH_
#define KTRPTRACKDATA_HH_

#include "KTData.hh"
#include "KTProcessedTrackData.hh"

#include "KTMemberVariable.hh"

#include <vector>
#include <map>

namespace Katydid
{
    
    class KTRPTrackData : public KTProcessedTrackData
    {

        public:
            KTRPTrackData();
            virtual ~KTRPTrackData();

            KTRPTrackData& operator=(const KTProcessedTrackData& rhs);

        public:

            MEMBERVARIABLE(bool, IsValid);   // did the fit converge?

            MEMBERVARIABLE(int, MainPeak);  // classifier for testing purposes
            MEMBERVARIABLE(unsigned, NPeaks);    // number of peaks in the fit

            // First four moments, first two in MHz
            MEMBERVARIABLE(double, Average); // adjusted for track intercept, i.e. a perfect track should give 0
            MEMBERVARIABLE(double, RMS);
            MEMBERVARIABLE(double, Skewness);
            MEMBERVARIABLE(double, Kurtosis);

            // Gaussian fit parameters of the peak closest to 0 (central peak)
            MEMBERVARIABLE(double, NormCentral);
            MEMBERVARIABLE(double, MeanCentral);
            MEMBERVARIABLE(double, SigmaCentral);
            MEMBERVARIABLE(double, MaximumCentral);
            MEMBERVARIABLE(double, RMSAwayFromCentral); // RMS in bins greater than 3 sigma from the central peak
            MEMBERVARIABLE(double, CentralPowerFraction); // Ratio of average power within 3 sigma of the central peak to average power greater than 3 sigma from the central peak

        public:
            static const std::string sName;
    };

} /* namespace Katydid */
#endif /* KTRPTRACKDATA_HH_ */
