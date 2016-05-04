/*
 * KTMultiTrackEventData.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#include "KTMultiTrackEventData.hh"

#include "KTLogger.hh"

#include <algorithm>
#include <cmath>

namespace Katydid
{
    KTLOGGER(evlog, "KTMultiTrackEventData")

    const std::string KTMultiTrackEventData::sName("multi-track-event");

    KTMultiTrackEventData::KTMultiTrackEventData() :
            KTExtensibleData< KTMultiTrackEventData >(),
            fComponent(0),
            fAcquisitionID(0.),
            fEventID(0),
            fTotalEventSequences(0),
            fStartTimeInAcq(0.),
            fStartTimeInRunC(0.),
            fEndTimeInRunC(0.),
            fTimeLength(0.),
            fStartFrequency(0.),
            fEndFrequency(0.),
            fMinimumFrequency(0.),
            fMaximumFrequency(0.),
            fFrequencyWidth(0.),
            fStartTimeInRunCSigma(0.),
            fEndTimeInRunCSigma(0.),
            fTimeLengthSigma(0.),
            fStartFrequencySigma(0.),
            fEndFrequencySigma(0.),
            fFrequencyWidthSigma(0.),
            fFirstTrackID(0),
            fFirstTrackTimeLength(0.),
            fFirstTrackFrequencyWidth(0.),
            fFirstTrackSlope(0.),
            fFirstTrackIntercept(0.),
            fFirstTrackTotalPower(0.),
            fUnknownEventTopology(false),
            fTracks()
    {
    }

    KTMultiTrackEventData::KTMultiTrackEventData(const KTMultiTrackEventData& orig) :
            KTExtensibleData< KTMultiTrackEventData >(orig),
            fComponent(orig.fComponent),
            fEventID(orig.fEventID),
            fTotalEventSequences(orig.fTotalEventSequences),
            fStartTimeInAcq(orig.fStartTimeInAcq),
            fStartTimeInRunC(orig.fStartTimeInRunC),
            fEndTimeInRunC(orig.fEndTimeInRunC),
            fTimeLength(orig.fTimeLength),
            fStartFrequency(orig.fStartFrequency),
            fEndFrequency(orig.fEndFrequency),
            fMinimumFrequency(orig.fMinimumFrequency),
            fMaximumFrequency(orig.fMaximumFrequency),
            fFrequencyWidth(orig.fFrequencyWidth),
            fStartTimeInRunCSigma(orig.fStartTimeInRunCSigma),
            fEndTimeInRunCSigma(orig.fEndTimeInRunCSigma),
            fTimeLengthSigma(orig.fTimeLengthSigma),
            fStartFrequencySigma(orig.fStartFrequencySigma),
            fEndFrequencySigma(orig.fEndFrequencySigma),
            fFrequencyWidthSigma(orig.fFrequencyWidthSigma),
            fFirstTrackID(orig.fFirstTrackID),
            fFirstTrackTimeLength(orig.fFirstTrackTimeLength),
            fFirstTrackFrequencyWidth(orig.fFirstTrackFrequencyWidth),
            fFirstTrackSlope(orig.fFirstTrackSlope),
            fFirstTrackIntercept(orig.fFirstTrackIntercept),
            fFirstTrackTotalPower(orig.fFirstTrackTotalPower),
            fTracks()
    {
        for (TrackCIt trackIt = orig.GetTracksBegin(); trackIt != orig.GetTracksEnd(); ++trackIt)
        {
            fTracks.insert(Tracks::value_type(trackIt->first, trackIt->second));
        }
    }

    KTMultiTrackEventData::~KTMultiTrackEventData()
    {
    }

    KTMultiTrackEventData& KTMultiTrackEventData::operator=(const KTMultiTrackEventData& rhs)
    {
        ClearTracks();

        fComponent = rhs.fComponent;

        fStartTimeInAcq = rhs.fStartTimeInAcq;
        fStartTimeInRunC = rhs.fStartTimeInRunC;
        fEndTimeInRunC = rhs.fEndTimeInRunC;
        fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency;
        fEndFrequency = rhs.fEndFrequency;
        fMinimumFrequency = rhs.fMinimumFrequency;
        fMaximumFrequency = rhs.fMaximumFrequency;
        fFrequencyWidth = rhs.fFrequencyWidth;

        fStartTimeInRunCSigma = rhs.fStartTimeInRunCSigma;
        fEndTimeInRunCSigma = rhs.fEndTimeInRunCSigma;
        fTimeLengthSigma = rhs.fTimeLengthSigma;
        fStartFrequencySigma = rhs.fStartFrequencySigma;
        fEndFrequencySigma = rhs.fEndFrequencySigma;
        fFrequencyWidthSigma = rhs.fFrequencyWidthSigma;

        fFirstTrackID = rhs.fFirstTrackID;
        fFirstTrackTimeLength = rhs.fFirstTrackTimeLength;
        fFirstTrackFrequencyWidth = rhs.fFirstTrackFrequencyWidth;
        fFirstTrackSlope = rhs.fFirstTrackSlope;
        fFirstTrackIntercept = rhs.fFirstTrackIntercept;
        fFirstTrackTotalPower = rhs.fFirstTrackTotalPower;

        for (TrackCIt trackIt = rhs.GetTracksBegin(); trackIt != rhs.GetTracksEnd(); ++trackIt)
        {
            fTracks.insert(Tracks::value_type(trackIt->first, trackIt->second));
        }

        return *this;
    }

    void KTMultiTrackEventData::AddTrack(const KTProcessedTrackData& track)
    {
        fTracks.insert(Tracks::value_type(track.GetTrackID(), track));
        return;
    }

    void KTMultiTrackEventData::AddTracks(TrackSet tracks)
    {
        AddTracks(tracks, fTotalEventSequences);
        fTotalEventSequences++;
        return;
    }

    void KTMultiTrackEventData::AddTracks(TrackSet tracks, ssize_t eventSequenceID)
    {
        for ( TrackSet::iterator aTrack=tracks.begin(); aTrack != tracks.end(); ++aTrack )
        {
            *aTrack->SetEventSequenceID(eventSequenceID);
        }
        return;
    }

    void KTMultiTrackEventData::ProcessTracks()
    {
        KTDEBUG(evlog, "Processing tracks");

        TrackCIt trackIt = fTracks.begin();

        fStartTimeInAcq = trackIt->second.GetStartTimeInAcq();
        fStartTimeInRunC = trackIt->second.GetStartTimeInRunC();
        fEndTimeInRunC = trackIt->second.GetEndTimeInRunC();
        fTimeLength = trackIt->second.GetTimeLength();
        fStartFrequency = trackIt->second.GetStartFrequency();
        fEndFrequency = trackIt->second.GetEndFrequency();
        fMinimumFrequency = std::min(fStartFrequency, fEndFrequency);
        fMaximumFrequency = std::max(fStartFrequency, fEndFrequency);
        fFrequencyWidth = trackIt->second.GetFrequencyWidth();

        fStartTimeInRunCSigma = trackIt->second.GetStartTimeInRunCSigma();
        fEndTimeInRunCSigma = trackIt->second.GetEndTimeInRunCSigma();
        fTimeLengthSigma = trackIt->second.GetTimeLengthSigma();
        fStartFrequencySigma = trackIt->second.GetStartFrequencySigma();
        fEndFrequencySigma = trackIt->second.GetEndFrequencySigma();
        fFrequencyWidthSigma = trackIt->second.GetFrequencyWidthSigma();

        fFirstTrackID = trackIt->first;
        fFirstTrackTimeLength = trackIt->second.GetTimeLength();
        fFirstTrackFrequencyWidth = trackIt->second.GetFrequencyWidth();
        fFirstTrackSlope = trackIt->second.GetSlope();
        fFirstTrackIntercept = trackIt->second.GetIntercept();
        fFirstTrackTotalPower = trackIt->second.GetTotalPower();

        for (++trackIt; trackIt != fTracks.end(); ++trackIt)
        {
            KTDEBUG(evlog, "Track " << trackIt->first);

            if (trackIt->second.GetStartTimeInRunC() < fStartTimeInRunC)
            {
                fStartTimeInAcq = trackIt->second.GetStartTimeInAcq();
                fStartTimeInRunC = trackIt->second.GetStartTimeInRunC();
                fStartTimeInRunCSigma = trackIt->second.GetStartTimeInRunCSigma();
                fStartFrequency = trackIt->second.GetStartFrequency();
                fStartFrequencySigma = trackIt->second.GetStartFrequencySigma();
                KTDEBUG(evlog, "Start time (freq) is now " << fStartTimeInRunC << "(" << fStartFrequency << ")");
                fFirstTrackID = trackIt->first;
                fFirstTrackTimeLength = trackIt->second.GetTimeLength();
                fFirstTrackFrequencyWidth = trackIt->second.GetFrequencyWidth();
                fFirstTrackSlope = trackIt->second.GetSlope();
                fFirstTrackIntercept = trackIt->second.GetIntercept();
                fFirstTrackTotalPower = trackIt->second.GetTotalPower();
            }

            if (trackIt->second.GetEndTimeInRunC() > fEndTimeInRunC)
            {
                fEndTimeInRunC = trackIt->second.GetEndTimeInRunC();
                fEndTimeInRunCSigma = trackIt->second.GetEndTimeInRunCSigma();
                fEndFrequency = trackIt->second.GetEndFrequency();
                fEndFrequencySigma = trackIt->second.GetEndFrequencySigma();
                KTDEBUG(evlog, "End time (freq) is now " << fEndTimeInRunC << "(" << fEndFrequency << ")");
            }

            double minFreq = std::min(trackIt->second.GetStartFrequency(), trackIt->second.GetEndFrequency());
            if (minFreq < fMinimumFrequency)
            {
                fMinimumFrequency = minFreq;
                KTDEBUG(evlog, "Minimum frequency is now " << fMinimumFrequency);
            }

            double maxFreq = std::max(trackIt->second.GetStartFrequency(), trackIt->second.GetEndFrequency());
            if (maxFreq > fMaximumFrequency)
            {
                fMaximumFrequency = maxFreq;
                KTDEBUG(evlog, "Maximum frequency is now " << fMaximumFrequency);
            }
        }

        fTimeLength = fEndTimeInRunC - fStartTimeInRunC;
        fTimeLengthSigma = sqrt(fEndTimeInRunCSigma * fEndTimeInRunCSigma + fStartTimeInRunCSigma * fStartTimeInRunCSigma);

        fFrequencyWidth = fEndFrequency - fStartFrequency;
        fFrequencyWidthSigma = sqrt(fEndFrequencySigma * fEndFrequencySigma + fStartFrequencySigma * fStartFrequencySigma);

        return;
    }

    void KTMultiTrackEventData::ClearTracks()
    {
        fTracks.clear();

        fStartTimeInAcq = 0.;
        fStartTimeInRunC = 0.;
        fEndTimeInRunC = 0.;
        fTimeLength = 0.;
        fStartFrequency = 0.;
        fEndFrequency = 0.;
        fMinimumFrequency = 0.;
        fMaximumFrequency = 0.;
        fFrequencyWidth = 0.;
        fStartTimeInRunCSigma = 0.;
        fEndTimeInRunCSigma = 0.;
        fTimeLengthSigma = 0.;
        fStartFrequencySigma = 0.;
        fEndFrequencySigma = 0.;
        fFrequencyWidthSigma = 0.;
        fFirstTrackTimeLength = 0.;
        fFirstTrackFrequencyWidth = 0.;
        fFirstTrackSlope = 0.;
        fFirstTrackIntercept = 0.;
        fFirstTrackTotalPower = 0.;

        return;
    }


} /* namespace Katydid */
