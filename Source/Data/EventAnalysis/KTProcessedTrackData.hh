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
    class KTProcessedTrackData : public Nymph::KTExtensibleData< KTProcessedTrackData >
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
            MEMBERVARIABLE(std::string, Filename);

            MEMBERVARIABLE(bool, IsCut);

            MEMBERVARIABLE(double, MVAClassifier);
            MEMBERVARIABLE(bool, Mainband);

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
            MEMBERVARIABLE(unsigned, NTrackBins);
            MEMBERVARIABLE(double, TotalTrackSNR);
            MEMBERVARIABLE(double, MaxTrackSNR);
            MEMBERVARIABLE(double, TotalTrackNUP);
            MEMBERVARIABLE(double, MaxTrackNUP);
            MEMBERVARIABLE(double, TotalWideTrackSNR);
            MEMBERVARIABLE(double, TotalWideTrackNUP);

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

    struct AllTrackData
    {
        Nymph::KTDataPtr fData;
        KTProcessedTrackData& fProcTrack;

        AllTrackData( Nymph::KTDataPtr data, KTProcessedTrackData& track ) : fData( data ), fProcTrack( track ) {}
        AllTrackData( Nymph::KTDataPtr data ) : fData( data ), fProcTrack( data->Of< KTProcessedTrackData >() ) {}
    };

    // containers of KTProcessedTrackData
    struct TrackTimeComp
    {
        bool operator() (const AllTrackData& lhs, const AllTrackData& rhs) const
        {
            if (lhs.fProcTrack.GetEventSequenceID() != rhs.fProcTrack.GetEventSequenceID()) return lhs.fProcTrack.GetEventSequenceID() < rhs.fProcTrack.GetEventSequenceID();
            if (lhs.fProcTrack.GetStartTimeInRunC() != rhs.fProcTrack.GetStartTimeInRunC()) return lhs.fProcTrack.GetStartTimeInRunC() < rhs.fProcTrack.GetStartTimeInRunC();
            if (lhs.fProcTrack.GetEndTimeInRunC() != rhs.fProcTrack.GetEndTimeInRunC()) return lhs.fProcTrack.GetEndTimeInRunC() < rhs.fProcTrack.GetEndTimeInRunC();
            if (lhs.fProcTrack.GetStartFrequency() != rhs.fProcTrack.GetStartFrequency()) return lhs.fProcTrack.GetStartFrequency() < rhs.fProcTrack.GetStartFrequency();
            return lhs.fProcTrack.GetEndFrequency() < rhs.fProcTrack.GetEndFrequency();
        }
    };

    typedef std::set< AllTrackData, TrackTimeComp > TrackSet;
    typedef TrackSet::iterator TrackSetIt;
    typedef TrackSet::const_iterator TrackSetCIt;

    struct TrackSetCItComp
    {
        bool operator() (const TrackSetCIt& lhs, const TrackSetCIt& rhs) const
        {
            if (lhs->fProcTrack.GetEventSequenceID() != rhs->fProcTrack.GetEventSequenceID()) return lhs->fProcTrack.GetEventSequenceID() < rhs->fProcTrack.GetEventSequenceID();
            if (lhs->fProcTrack.GetStartTimeInRunC() != rhs->fProcTrack.GetStartTimeInRunC()) return lhs->fProcTrack.GetStartTimeInRunC() < rhs->fProcTrack.GetStartTimeInRunC();
            if (lhs->fProcTrack.GetEndTimeInRunC() != rhs->fProcTrack.GetEndTimeInRunC()) return lhs->fProcTrack.GetEndTimeInRunC() < rhs->fProcTrack.GetEndTimeInRunC();
            if (lhs->fProcTrack.GetStartFrequency() != rhs->fProcTrack.GetStartFrequency()) return lhs->fProcTrack.GetStartFrequency() < rhs->fProcTrack.GetStartFrequency();
            return lhs->fProcTrack.GetEndFrequency() < rhs->fProcTrack.GetEndFrequency();
        }
    };

    typedef std::set< TrackSetCIt, TrackSetCItComp > TrackSetCItSet;

} /* namespace Katydid */
#endif /* KTPROCESSEDTRACKDATA_HH_ */
