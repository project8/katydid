/*
 * KTMultiTrackEventData.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#include "KTMultiTrackEventData.hh"

#include <cmath>

namespace Katydid
{
    KTMultiTrackEventData::KTMultiTrackEventData() :
            KTExtensibleData< KTMultiTrackEventData >(),
            fComponent(0),
            fEventID(0),
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
            fFirstTrackTimeLength(0.),
            fFirstTrackFrequencyWidth(0.),
            fFirstTrackSlope(0.),
            fFirstTrackIntercept(0.),
            fFirstTrackTotalPower(0.),
            fTracks()
    {
    }

    KTMultiTrackEventData::KTMultiTrackEventData(const KTMultiTrackEventData& orig) :
            KTExtensibleData< KTMultiTrackEventData >(orig),
            fComponent(orig.fComponent),
            fEventID(orig.fEventID),
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
            fTracks.insert(Tracks::value_type(trackIt->first, trackIt->second));
        }

        return *this;
    }

    void KTMultiTrackEventData::AddTrack(const KTProcessedTrackData& track)
    {
        fTracks.insert(Tracks::value_type(track.GetTrackID(), track));

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

    void KTMultiTrackEventData::ClearTracks()
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
