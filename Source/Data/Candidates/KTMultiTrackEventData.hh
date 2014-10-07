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

namespace Katydid
{
    class KTMultiTrackEventData : public KTExtensibleData< KTMultiTrackEventData >
    {
        public:
            typedef std::map< unsigned, KTProcessedTrackData > Tracks;
            typedef Tracks::iterator TrackIt;
            typedef Tracks::const_iterator TrackCIt;

        public:
            KTMultiTrackEventData();
            KTMultiTrackEventData(const KTMultiTrackEventData& orig);
            virtual ~KTMultiTrackEventData();

            KTMultiTrackEventData& operator=(const KTMultiTrackEventData& rhs);

            MEMBERVARIABLE(unsigned, Component);
            MEMBERVARIABLE(unsigned, EventID);

            // this group of member variables is set by ProcessTracks()
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

            unsigned GetNTracks() const;

            bool HasTrack(unsigned id) const;
            const KTProcessedTrackData& GetTrack(unsigned id) const;
            KTProcessedTrackData& GetTrack(unsigned id);

            void AddTrack(const KTProcessedTrackData& track);
            /// Evaluates all of the tracks to fill in information about the event and first track
            void ProcessTracks();

            /// Removes all track data, as well as collective time and frequency info
            void ClearTracks();

            TrackCIt GetTracksBegin() const;
            TrackIt GetTracksBegin();

            TrackCIt GetTracksEnd() const;
            TrackIt GetTracksEnd();

        private:
            Tracks fTracks;

        public:
            static const std::string sName;
};

    inline unsigned KTMultiTrackEventData::KTMultiTrackEventData::GetNTracks() const
    {
        return fTracks.size();
    }

    inline const KTProcessedTrackData& KTMultiTrackEventData::GetTrack(unsigned id) const
    {
        return fTracks.at(id);
    }

    inline KTProcessedTrackData& KTMultiTrackEventData::GetTrack(unsigned id)
    {
        return fTracks.at(id);
    }

    inline KTMultiTrackEventData::TrackCIt KTMultiTrackEventData::GetTracksBegin() const
    {
        return fTracks.begin();
    }

    inline KTMultiTrackEventData::TrackIt KTMultiTrackEventData::GetTracksBegin()
    {
        return fTracks.begin();
    }

    inline KTMultiTrackEventData::TrackCIt KTMultiTrackEventData::GetTracksEnd() const
    {
        return fTracks.end();
    }

    inline KTMultiTrackEventData::TrackIt KTMultiTrackEventData::GetTracksEnd()
    {
        return fTracks.end();
    }


} /* namespace Katydid */
#endif /* KTMULTITRACKDATA_HH_ */
