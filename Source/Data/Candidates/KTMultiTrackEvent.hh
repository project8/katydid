/*
 * KTMultiTrackEvent.hh
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#ifndef KTMULTITRACKDATA_HH_
#define KTMULTITRACKDATA_HH_

#include "KTData.hh"

#include "KTProcessedTrackData.hh"

#include <vector>

namespace Katydid
{
    class KTMultiTrackEvent : public KTExtensibleData< KTMultiTrackEvent >
    {
        public:
            typedef std::vector< KTProcessedTrackData > Tracks;
            typedef Tracks::iterator TrackIt;
            typedef Tracks::const_iterator TrackCIt;

        public:
            KTMultiTrackEvent();
            KTMultiTrackEvent(const KTMultiTrackEvent& orig);
            virtual ~KTMultiTrackEvent();

            KTMultiTrackEvent& operator=(const KTMultiTrackEvent& rhs);

            unsigned GetComponent() const;

            double GetStartTimeInRunC() const;
            double GetEndTimeInRunC() const;
            double GetTimeLength() const;
            double GetStartFrequency() const;
            double GetEndFrequency() const;
            double GetFrequencyWidth() const;

            double GetStartTimeInRunCSigma() const;
            double GetEndTimeInRunCSigma() const;
            double GetTimeLengthSigma() const;
            double GetStartFrequencySigma() const;
            double GetEndFrequencySigma() const;
            double GetFrequencyWidthSigma() const;

            unsigned GetNTracks() const;

            const KTProcessedTrackData& GetTrack(unsigned iTrack) const;
            KTProcessedTrackData& GetTrack(unsigned iTrack);

            TrackCIt GetTracksBegin() const;
            TrackIt GetTracksBegin();

            TrackCIt GetTracksEnd() const;
            TrackIt GetTracksEnd();

            void SetComponent(unsigned component);

            void SetStartTimeInRunC(double tirc);
            void SetEndTimeInRunC(double tirc);
            void SetTimeLength(double length);
            void SetStartFrequency(double freq);
            void SetEndFrequency(double freq);
            void SetFrequencyWidth(double width);

            void SetStartTimeInRunCSigma(double sigma);
            void SetEndTimeInRunCSigma(double sigma);
            void SetTimeLengthSigma(double sigma);
            void SetStartFrequencySigma(double sigma);
            void SetEndFrequencySigma(double sigma);
            void SetFrequencyWidthSigma(double sigma);

            void AddTrack(const KTProcessedTrackData& track);

            /// Removes all track data, as well as collective time and frequency info
            void ClearTracks();

        protected:
            unsigned fComponent;

            double fStartTimeInRunC;
            double fEndTimeInRunC;
            double fTimeLength;
            double fStartFrequency;
            double fEndFrequency;
            double fFrequencyWidth;

            double fStartTimeInRunCSigma;
            double fEndTimeInRunCSigma;
            double fTimeLengthSigma;
            double fStartFrequencySigma;
            double fEndFrequencySigma;
            double fFrequencyWidthSigma;

            Tracks fTracks;
    };

    inline unsigned KTMultiTrackEvent::GetComponent() const
    {
        return fComponent;
    }

    inline double KTMultiTrackEvent::GetStartTimeInRunC() const
    {
        return fStartTimeInRunC;
    }

    inline double KTMultiTrackEvent::GetEndTimeInRunC() const
    {
        return fEndTimeInRunC;
    }

    inline double KTMultiTrackEvent::GetTimeLength() const
    {
        return fTimeLength;
    }

    inline double KTMultiTrackEvent::GetStartFrequency() const
    {
        return fStartFrequency;
    }

    inline double KTMultiTrackEvent::GetEndFrequency() const
    {
        return fEndFrequency;
    }

    inline double KTMultiTrackEvent::GetFrequencyWidth() const
    {
        return fFrequencyWidth;
    }

    inline double KTMultiTrackEvent::GetStartTimeInRunCSigma() const
    {
        return fStartTimeInRunCSigma;
    }

    inline double KTMultiTrackEvent::GetEndTimeInRunCSigma() const
    {
        return fEndTimeInRunCSigma;
    }

    inline double KTMultiTrackEvent::GetTimeLengthSigma() const
    {
        return fTimeLengthSigma;
    }

    inline double KTMultiTrackEvent::GetStartFrequencySigma() const
    {
        return fStartFrequencySigma;
    }

    inline double KTMultiTrackEvent::GetEndFrequencySigma() const
    {
        return fEndFrequencySigma;
    }

    inline double KTMultiTrackEvent::GetFrequencyWidthSigma() const
    {
        return fFrequencyWidthSigma;
    }

    inline unsigned KTMultiTrackEvent::KTMultiTrackEvent::GetNTracks() const
    {
        return fTracks.size();
    }

    inline const KTProcessedTrackData& KTMultiTrackEvent::GetTrack(unsigned iTrack) const
    {
        return fTracks[iTrack];
    }

    inline KTProcessedTrackData& KTMultiTrackEvent::GetTrack(unsigned iTrack)
    {
        return fTracks[iTrack];
    }

    inline KTMultiTrackEvent::TrackCIt KTMultiTrackEvent::GetTracksBegin() const
    {
        return fTracks.begin();
    }

    inline KTMultiTrackEvent::TrackIt KTMultiTrackEvent::GetTracksBegin()
    {
        return fTracks.begin();
    }

    inline KTMultiTrackEvent::TrackCIt KTMultiTrackEvent::GetTracksEnd() const
    {
        return fTracks.end();
    }

    inline KTMultiTrackEvent::TrackIt KTMultiTrackEvent::GetTracksEnd()
    {
        return fTracks.end();
    }

    inline void KTMultiTrackEvent::SetComponent(unsigned component)
    {
        fComponent = component;
        return;
    }

    inline void KTMultiTrackEvent::SetStartTimeInRunC(double tirc)
    {
        fStartTimeInRunC = tirc;
        return;
    }

    inline void KTMultiTrackEvent::SetEndTimeInRunC(double tirc)
    {
        fEndTimeInRunC = tirc;
        return;
    }

    inline void KTMultiTrackEvent::SetTimeLength(double length)
    {
        fTimeLength = length;
        return;
    }

    inline void KTMultiTrackEvent::SetStartFrequency(double freq)
    {
        fStartFrequency = freq;
        return;
    }

    inline void KTMultiTrackEvent::SetEndFrequency(double freq)
    {
        fEndFrequency = freq;
        return;
    }

    inline void KTMultiTrackEvent::SetFrequencyWidth(double width)
    {
        fFrequencyWidth = width;
        return;
    }

    inline void KTMultiTrackEvent::SetStartTimeInRunCSigma(double sigma)
    {
        fStartTimeInRunCSigma = sigma;
        return;
    }

    inline void KTMultiTrackEvent::SetEndTimeInRunCSigma(double sigma)
    {
        fEndTimeInRunCSigma = sigma;
        return;
    }

    inline void KTMultiTrackEvent::SetTimeLengthSigma(double sigma)
    {
        fTimeLengthSigma = sigma;
        return;
    }

    inline void KTMultiTrackEvent::SetStartFrequencySigma(double sigma)
    {
        fStartFrequencySigma = sigma;
        return;
    }

    inline void KTMultiTrackEvent::SetEndFrequencySigma(double sigma)
    {
        fEndFrequencySigma = sigma;
        return;
    }

    inline void KTMultiTrackEvent::SetFrequencyWidthSigma(double sigma)
    {
        fFrequencyWidthSigma = sigma;
        return;
    }

} /* namespace Katydid */
#endif /* KTMULTITRACKDATA_HH_ */
