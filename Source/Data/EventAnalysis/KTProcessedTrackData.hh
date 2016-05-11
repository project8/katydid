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

#include <set>

namespace Katydid
{
    using namespace Nymph;
    class KTProcessedTrackData : public KTExtensibleData< KTProcessedTrackData >
    {
        public:
            KTProcessedTrackData();
            KTProcessedTrackData(const KTProcessedTrackData& orig);
            virtual ~KTProcessedTrackData();

            KTProcessedTrackData& operator=(const KTProcessedTrackData& rhs);

            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLE(uint64_t, AcquisitionID);
            MEMBERVARIABLE(unsigned, TrackID);
            MEMBERVARIABLE(unsigned, EventID);
            MEMBERVARIABLE(ssize_t, EventSequenceID);

            MEMBERVARIABLE(bool, IsCut);

            MEMBERVARIABLE(double, StartTimeInAcq);
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

        public:
            static const std::string sName;

    };

    // containers of KTProcessedTrackData
    struct TrackTimeComp
    {
        bool operator() (const KTProcessedTrackData& lhs, const KTProcessedTrackData& rhs) const
        {
            if (lhs.GetEventSequenceID() != rhs.GetEventSequenceID()) return lhs.GetEventSequenceID() < rhs.GetEventSequenceID();
            if (lhs.GetStartTimeInRunC() != rhs.GetStartTimeInRunC()) return lhs.GetStartTimeInRunC() < rhs.GetStartTimeInRunC();
            if (lhs.GetEndTimeInRunC() != rhs.GetEndTimeInRunC()) return lhs.GetEndTimeInRunC() < rhs.GetEndTimeInRunC();
            if (lhs.GetStartFrequency() != rhs.GetStartFrequency()) return lhs.GetStartFrequency() < rhs.GetStartFrequency();
            return lhs.GetEndFrequency() < rhs.GetEndFrequency();
        }
    };
    typedef std::set< KTProcessedTrackData, TrackTimeComp > TrackSet;
    typedef TrackSet::iterator TrackSetIt;
    typedef TrackSet::const_iterator TrackSetCIt;
    struct TrackSetCItComp
    {
        bool operator() (const TrackSetCIt& lhs, const TrackSetCIt& rhs) const
        {
            if (lhs->GetEventSequenceID() != rhs->GetEventSequenceID()) return lhs->GetEventSequenceID() < rhs->GetEventSequenceID();
            if (lhs->GetStartTimeInRunC() != rhs->GetStartTimeInRunC()) return lhs->GetStartTimeInRunC() < rhs->GetStartTimeInRunC();
            if (lhs->GetEndTimeInRunC() != rhs->GetEndTimeInRunC()) return lhs->GetEndTimeInRunC() < rhs->GetEndTimeInRunC();
            if (lhs->GetStartFrequency() != rhs->GetStartFrequency()) return lhs->GetStartFrequency() < rhs->GetStartFrequency();
            return lhs->GetEndFrequency() < rhs->GetEndFrequency();
        }
    };
    typedef std::set< TrackSetCIt, TrackSetCItComp > TrackSetCItSet;

} /* namespace Katydid */
#endif /* KTPROCESSEDTRACKDATA_HH_ */
