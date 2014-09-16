/*
 * KTProcessedTrackData.hh
 *
 *  Created on:July 22, 2014
 *      Author: nsoblath
 */

#ifndef KTPROCESSEDTRACKDATA_HH_
#define KTPROCESSEDTRACKDATA_HH_

#include "KTData.hh"

#include "KTMemberVariable.hh"

namespace Katydid
{
    class KTProcessedTrackData : public KTExtensibleData< KTProcessedTrackData >
    {
        public:
            KTProcessedTrackData();
            KTProcessedTrackData(const KTProcessedTrackData& orig);
            virtual ~KTProcessedTrackData();

            KTProcessedTrackData& operator=(const KTProcessedTrackData& rhs);

            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLE(unsigned, TrackID);

            MEMBERVARIABLE(bool, IsCut);

            MEMBERVARIABLE(double, StartTimeInRunC);
            MEMBERVARIABLE(double, EndTimeInRunC);
            MEMBERVARIABLE(double, TimeLength);
            MEMBERVARIABLE(double, StartFrequency);
            MEMBERVARIABLE(double, EndFrequency);
            MEMBERVARIABLE(double, FrequencyWidth);
            MEMBERVARIABLE(double, Slope);
            MEMBERVARIABLE(double, Intercept);
            MEMBERVARIABLE(double, TotalPower);

            MEMBERVARIABLE(double, StartTimeInRunCSigma);
            MEMBERVARIABLE(double, EndTimeInRunCSigma);
            MEMBERVARIABLE(double, TimeLengthSigma);
            MEMBERVARIABLE(double, StartFrequencySigma);
            MEMBERVARIABLE(double, EndFrequencySigma);
            MEMBERVARIABLE(double, FrequencyWidthSigma);
            MEMBERVARIABLE(double, SlopeSigma);
            MEMBERVARIABLE(double, InterceptSigma);
            MEMBERVARIABLE(double, TotalPowerSigma);

        private:
            static const std::string sName;

    };

} /* namespace Katydid */
#endif /* KTPROCESSEDTRACKDATA_HH_ */
