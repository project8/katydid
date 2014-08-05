/*
 * KTMultiTrackEvent.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#include "KTMultiTrackEvent.hh"

namespace Katydid
{
    KTMultiTrackEvent::KTMultiTrackEvent() :
            KTExtensibleData< KTMultiTrackEvent >(),
            fComponent(0),
            fStartTimeInRunC(0.),
            fEndTimeInRunC(0.),
            fTimeLength(0.),
            fStartFrequency(0.),
            fEndFrequency(0.),
            fFrequencyWidth(0.),
            fStartTimeInRunCSigma(0.),
            fEndTimeInRunCSigma(0.),
            fTimeLengthSigma(0.),
            fStartFrequencySigma(0.),
            fEndFrequencySigma(0.),
            fFrequencyWidthSigma(0.),
            fTracks()
    {
    }

    KTMultiTrackEvent::KTMultiTrackEvent(const KTMultiTrackEvent& orig) :
            KTExtensibleData< KTMultiTrackEvent >(orig),
            fComponent(orig.fComponent),
            fStartTimeInRunC(orig.fStartTimeInRunC),
            fEndTimeInRunC(orig.fEndTimeInRunC),
            fTimeLength(orig.fTimeLength),
            fStartFrequency(orig.fStartFrequency),
            fEndFrequency(orig.fEndFrequency),
            fFrequencyWidth(orig.fFrequencyWidth),
            fStartTimeInRunCSigma(orig.fStartTimeInRunCSigma),
            fEndTimeInRunCSigma(orig.fEndTimeInRunCSigma),
            fTimeLengthSigma(orig.fTimeLengthSigma),
            fStartFrequencySigma(orig.fStartFrequencySigma),
            fEndFrequencySigma(orig.fEndFrequencySigma),
            fFrequencyWidthSigma(orig.fFrequencyWidthSigma),
            fTracks()
    {
        for (TrackCIt trackIt = orig.GetTracksBegin(); trackIt != orig.GetTracksEnd(); ++trackIt)
        {
            fTracks.push_back(*trackIt);
        }
    }

    KTMultiTrackEvent::~KTMultiTrackEvent()
    {
    }

    KTMultiTrackEvent& KTMultiTrackEvent::operator=(const KTMultiTrackEvent& rhs)
    {
        ClearTracks();

        fComponent = rhs.fComponent;

        fStartTimeInRunC = rhs.fStartTimeInRunC;
        fEndTimeInRunC = rhs.fEndTimeInRunC;
        fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency;
        fEndFrequency = rhs.fEndFrequency;
        fFrequencyWidth = rhs.fFrequencyWidth;

        fStartTimeInRunCSigma = rhs.fStartTimeInRunCSigma;
        fEndTimeInRunCSigma = rhs.fEndTimeInRunCSigma;
        fTimeLengthSigma = rhs.fTimeLengthSigma;
        fStartFrequencySigma = rhs.fStartFrequencySigma;
        fEndFrequencySigma = rhs.fEndFrequencySigma;
        fFrequencyWidthSigma = rhs.fFrequencyWidthSigma;

        for (TrackCIt trackIt = rhs.GetTracksBegin(); trackIt != rhs.GetTracksEnd(); ++trackIt)
        {
            fTracks.push_back(*trackIt);
        }

        return *this;
    }

    void KTMultiTrackEvent::AddTrack(const KTProcessedTrackData& track)
    {
        fTracks.push_back(track);

        bool updateTimeLength = false;
        if (track.GetStartTimeInRunC() < fStartTimeInRunC)
        {
            fStartTimeInRunC = track.GetStartTimeInRunC();
            fStartTimeInRunCSigma = track.GetStartTimeInRunCSigma();
            updateTimeLength = true;
        }
        if (track.GetEndTimeInRunC() > fEndTimeInRunC)
        {
            fEndTimeInRunC = track.GetEndTimeInRunC();
            fEndTimeInRunCSigma = track.GetEndTimeInRunCSigma();
            updateTimeLength = true;
        }

        if (updateTimeLength)
        {
            fTimeLength = fEndTimeInRunC - fStartTimeInRunC;
            fTimeLengthSigma = sqrt(fEndTimeInRunC * fEndTimeInRunC + fStartTimeInRunC * fStartTimeInRunC);
        }

        bool updateFreqWidth = false;
        if (track.GetStartFrequency() < fStartFrequency)
        {
            fStartFrequency = track.GetStartFrequency();
            fStartFrequencySigma = track.GetStartFrequencySigma();
            updateFreqWidth = true;
        }
        if (track.GetEndTimeInRunC() > fEndFrequency)
        {
            fEndFrequency = track.GetEndFrequency();
            fEndFrequencySigma = track.GetEndFrequencySigma();
            updateFreqWidth = true;
        }

        if (updateFreqWidth)
        {
            fFrequencyWidth = fEndFrequency - fStartFrequency;
            fFrequencyWidthSigma = sqrt(fEndFrequency * fEndFrequency + fStartFrequency * fStartFrequency);
        }

        return;
    }

    void KTMultiTrackEvent::ClearTracks()
    {
        fTracks.clear();

        fStartTimeInRunC = 0.;
        fEndTimeInRunC = 0.;
        fTimeLength = 0.;
        fStartFrequency = 0.;
        fEndFrequency = 0.;
        fFrequencyWidth = 0.;
        fStartTimeInRunCSigma = 0.;
        fEndTimeInRunCSigma = 0.;
        fTimeLengthSigma = 0.;
        fStartFrequencySigma = 0.;
        fEndFrequencySigma = 0.;
        fFrequencyWidthSigma = 0.;

        return;
    }


} /* namespace Katydid */
