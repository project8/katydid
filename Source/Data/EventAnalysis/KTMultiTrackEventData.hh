/*
 * KTMultiTrackEventData.hh
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#ifndef KTMULTITRACKDATA_HH_
#define KTMULTITRACKDATA_HH_

#include "KTData.hh"

#include "KTMemberVariable.hh"
#include "KTProcessedTrackData.hh"

#include <map>
#include <set>

namespace Katydid
{
    class KTMultiTrackEventData : public Nymph::KTExtensibleData< KTMultiTrackEventData >
    {
        public:
            //typedef std::map< unsigned, KTProcessedTrackData > Tracks;
            //typedef TrackSet Tracks;
            //typedef Tracks::iterator TrackIt;
            //typedef Tracks::const_iterator TrackCIt;

        public:
            KTMultiTrackEventData();
            KTMultiTrackEventData(const KTMultiTrackEventData& orig);
            virtual ~KTMultiTrackEventData();

            KTMultiTrackEventData& operator=(const KTMultiTrackEventData& rhs);

            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLE(uint64_t, AcquisitionID);
            MEMBERVARIABLE(unsigned, EventID);
            MEMBERVARIABLE(unsigned, TotalEventSequences); // a sequence is a collection of lines with common start and end time, TotalEventSequences should be exactly the number of scatters plus one

            // this group of member variables is set by ProcessTracks()
            MEMBERVARIABLE(double, StartTimeInAcq);
            MEMBERVARIABLE(double, StartTimeInRunC);
            MEMBERVARIABLE(double, EndTimeInRunC);
            MEMBERVARIABLE(double, TimeLength);
            MEMBERVARIABLE(double, StartFrequency);
            MEMBERVARIABLE(double, EndFrequency);
            MEMBERVARIABLE(double, MinimumFrequency);
            MEMBERVARIABLE(double, MaximumFrequency);
            MEMBERVARIABLE(double, FrequencyWidth);

            // this group of member variables is set by ProcessTracks()
            MEMBERVARIABLE(double, StartTimeInRunCSigma);
            MEMBERVARIABLE(double, EndTimeInRunCSigma);
            MEMBERVARIABLE(double, TimeLengthSigma);
            MEMBERVARIABLE(double, StartFrequencySigma);
            MEMBERVARIABLE(double, EndFrequencySigma);
            MEMBERVARIABLE(double, FrequencyWidthSigma);

            // this group of member variables is set by ProcessTracks()
            MEMBERVARIABLE(unsigned, FirstTrackID);
            MEMBERVARIABLE(double, FirstTrackTimeLength);
            MEMBERVARIABLE(double, FirstTrackFrequencyWidth);
            MEMBERVARIABLE(double, FirstTrackSlope);
            MEMBERVARIABLE(double, FirstTrackIntercept);
            MEMBERVARIABLE(double, FirstTrackTotalPower);

            // this member variable is set by event building
            MEMBERVARIABLE(bool, UnknownEventTopology); // if True, indicates that the reconstruction was unable to deal with the event

        public:
            unsigned GetNTracks() const;

            bool HasTrack(unsigned id) const;
            //const KTProcessedTrackData& GetTrack(unsigned id) const;
            //KTProcessedTrackData& GetTrack(unsigned id);

            void AddTrack(const AllTrackData& track);
            void AddTrack(const KTProcessedTrackData& track);
            /// Add a collection of tracks to the next EventSequenceID
            void AddTracks(TrackSetCItSet tracks);
            void AddTracks(TrackSet tracks);
            /// Add a collection of tracks to the specified EventSequenceID
            void AddTracks(TrackSetCItSet tracks, ssize_t eventSequenceID);
            void AddTracks(TrackSet tracks, ssize_t eventSequenceID);

            /// Evaluates all of the tracks to fill in information about the event and first track
            void ProcessTracks();

            /// Removes all track data, as well as collective time and frequency info
            void ClearTracks();

            TrackSetCIt GetTracksBegin() const;
            TrackSetIt GetTracksBegin();

            TrackSetCIt GetTracksEnd() const;
            TrackSetIt GetTracksEnd();

            TrackSet GetTracksSet();

        private:
            //Tracks fTracks;
            TrackSet fTracks;

        public:
            static const std::string sName;
    };

    inline unsigned KTMultiTrackEventData::GetNTracks() const
    {
        return fTracks.size();
    }

    inline TrackSetCIt KTMultiTrackEventData::GetTracksBegin() const
    {
        return fTracks.begin();
    }

    inline TrackSetIt KTMultiTrackEventData::GetTracksBegin()
    {
        return fTracks.begin();
    }

    inline TrackSetCIt KTMultiTrackEventData::GetTracksEnd() const
    {
        return fTracks.end();
    }

    inline TrackSetIt KTMultiTrackEventData::GetTracksEnd()
    {
        return fTracks.end();
    }

    inline TrackSet KTMultiTrackEventData::GetTracksSet()
    {
        return fTracks;
    }

    struct MultiPeakTrackRef
    {
        std::set< TrackSetCIt, TrackSetCItComp > fTrackRefs;
        // Keep track of both the sum and the mean so that the mean can be updated regularly without an extra multiplication
        double fMeanStartTimeInRunC;
        double fSumStartTimeInRunC;
        double fMeanEndTimeInRunC;
        double fSumEndTimeInRunC;
        uint64_t fAcquisitionID;
        bool fUnknownEventTopology;

        MultiPeakTrackRef();
        bool InsertTrack(const TrackSetCIt& trackRef);
        void Clear();
    };

    struct MTRComp
    {
        bool operator() (const MultiPeakTrackRef& lhs, const MultiPeakTrackRef& rhs)
        {
            if (lhs.fMeanStartTimeInRunC != rhs.fMeanStartTimeInRunC) return lhs.fMeanStartTimeInRunC < rhs.fMeanStartTimeInRunC;
            return lhs.fMeanEndTimeInRunC < rhs.fMeanEndTimeInRunC;
        }

    };

    class KTMultiPeakTrackData : public Nymph::KTExtensibleData< KTMultiPeakTrackData >
    {

        private:

            MultiPeakTrackRef fMPTrack;
            unsigned fComponent;
            int fEventSequenceID;

        public:
            KTMultiPeakTrackData();
            KTMultiPeakTrackData(const KTMultiPeakTrackData& orig);
            virtual ~KTMultiPeakTrackData();

            KTMultiPeakTrackData& operator=(const KTMultiPeakTrackData& rhs);

        public:

            unsigned GetComponent() const;
            void SetComponent(unsigned component);

            MultiPeakTrackRef GetMPTrack() const;
            void SetMPTrack( const MultiPeakTrackRef& mpt );
            void AddToMPTrack( const TrackSetCIt& trackRef );

            int GetMultiplicity() const;

            int GetEventSequenceID() const;
            void SetEventSequenceID( int id );

            double GetMeanStartTimeInRunC() const;

            double GetSumStartTimeInRunC() const;

            double GetMeanEndTimeInRunC() const;

            double GetSumEndTimeInRunC() const;

            uint64_t GetAcquisitionID() const;
            void SetAcquisitionID(uint64_t acqID);

            bool GetUnknownEventTopology() const;
            void SetUnknownEventTopology(bool b);
 
        public:
            static const std::string sName;
    };

    std::ostream& operator<<(std::ostream& out, const KTMultiPeakTrackData& hdr);

    inline unsigned KTMultiPeakTrackData::GetComponent() const
    {
        return fComponent;
    }

    inline void KTMultiPeakTrackData::SetComponent(unsigned component)
    {
        fComponent = component;
    }

    inline MultiPeakTrackRef KTMultiPeakTrackData::GetMPTrack() const
    {
        return fMPTrack;
    }

    inline void KTMultiPeakTrackData::SetMPTrack( const MultiPeakTrackRef& mpt )
    {
        fMPTrack = mpt;
    }

    inline void KTMultiPeakTrackData::AddToMPTrack( const TrackSetCIt& trackRef )
    {
        fMPTrack.InsertTrack( trackRef );
    }

    inline int KTMultiPeakTrackData::GetMultiplicity() const
    {
        return fMPTrack.fTrackRefs.size();
    }

    inline int KTMultiPeakTrackData::GetEventSequenceID() const
    {
        return fEventSequenceID;
    }

    inline void KTMultiPeakTrackData::SetEventSequenceID(int id)
    {
        fEventSequenceID = id;
    }

    inline double KTMultiPeakTrackData::GetMeanStartTimeInRunC() const
    {
        return fMPTrack.fMeanStartTimeInRunC;
    }

    inline double KTMultiPeakTrackData::GetSumStartTimeInRunC() const
    {
        return fMPTrack.fSumStartTimeInRunC;
    }

    inline double KTMultiPeakTrackData::GetMeanEndTimeInRunC() const
    {
        return fMPTrack.fMeanEndTimeInRunC;
    }

    inline double KTMultiPeakTrackData::GetSumEndTimeInRunC() const
    {
        return fMPTrack.fSumEndTimeInRunC;
    }

    inline uint64_t KTMultiPeakTrackData::GetAcquisitionID() const
    {
        return fMPTrack.fAcquisitionID;
    }

    inline void KTMultiPeakTrackData::SetAcquisitionID(uint64_t acqID)
    {
        fMPTrack.fAcquisitionID = acqID;
    }

    inline bool KTMultiPeakTrackData::GetUnknownEventTopology() const
    {
        return fMPTrack.fUnknownEventTopology;
    }

    inline void KTMultiPeakTrackData::SetUnknownEventTopology(bool b)
    {
        fMPTrack.fUnknownEventTopology = b;
    }

} /* namespace Katydid */
#endif /* KTMULTITRACKDATA_HH_ */
