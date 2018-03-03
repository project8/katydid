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
            fAcquisitionID(orig.fAcquisitionID),
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
            fUnknownEventTopology(orig.fUnknownEventTopology),
            fTracks()
    {
        for (TrackSetCIt trackIt = orig.GetTracksBegin(); trackIt != orig.GetTracksEnd(); ++trackIt)
        {
            //fTracks.insert(Tracks::value_type(trackIt->first, trackIt->second));
            fTracks.insert(*trackIt);
        }
    }

    KTMultiTrackEventData::~KTMultiTrackEventData()
    {
    }

    KTMultiTrackEventData& KTMultiTrackEventData::operator=(const KTMultiTrackEventData& rhs)
    {
        ClearTracks();

        fComponent = rhs.fComponent;

        fAcquisitionID = rhs.fAcquisitionID;
        fEventID = rhs.fEventID;

        fTotalEventSequences = rhs.fTotalEventSequences;

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

        fUnknownEventTopology = rhs.fUnknownEventTopology;

        for (TrackSetCIt trackIt = rhs.GetTracksBegin(); trackIt != rhs.GetTracksEnd(); ++trackIt)
        {
            //fTracks.insert(Tracks::value_type(trackIt->first, trackIt->second));
            fTracks.insert(*trackIt);
        }

        return *this;
    }

    const std::string KTProcessedMPTData::sName("proc-mpt");

    KTProcessedMPTData::KTProcessedMPTData() :
            KTExtensibleData< KTProcessedMPTData >(),
            fComponent(0),
            fMainTrack(),
            fAxialFrequency(0.)
    {
    }

    KTProcessedMPTData::KTProcessedMPTData(const KTProcessedMPTData& orig) :
            KTExtensibleData< KTProcessedMPTData >(orig),
            
            fComponent(orig.fComponent),
            fMainTrack(orig.fMainTrack),
            fAxialFrequency(orig.fAxialFrequency)
    {
    }

    KTProcessedMPTData::~KTProcessedMPTData()
    {
    }

    KTProcessedMPTData& KTProcessedMPTData::operator=(const KTProcessedMPTData& rhs)
    {
        KTExtensibleData< KTProcessedMPTData >::operator=(rhs);
        fComponent = rhs.fComponent;
        fMainTrack = rhs.fMainTrack;
        fAxialFrequency = rhs.fAxialFrequency;
        return *this;
    }

    const std::string KTMultiPeakTrackData::sName("mpt-data");

    KTMultiPeakTrackData::KTMultiPeakTrackData() :
            KTExtensibleData< KTMultiPeakTrackData >(),
            fComponent(0),
            fEventSequenceID(-1),
            fMPTrack()
    {
    }

    KTMultiPeakTrackData::KTMultiPeakTrackData(const KTMultiPeakTrackData& orig) :
            KTExtensibleData< KTMultiPeakTrackData >(orig),
            
            fComponent(orig.fComponent),
            fEventSequenceID(orig.fEventSequenceID),
            fMPTrack(orig.fMPTrack)
    {
    }

    KTMultiPeakTrackData::~KTMultiPeakTrackData()
    {
    }

    KTMultiPeakTrackData& KTMultiPeakTrackData::operator=(const KTMultiPeakTrackData& rhs)
    {
        KTExtensibleData< KTMultiPeakTrackData >::operator=(rhs);
        fComponent = rhs.fComponent;
        fEventSequenceID = rhs.fEventSequenceID;
        fMPTrack = rhs.fMPTrack;
        return *this;
    }

    void KTMultiTrackEventData::AddTrack(const AllTrackData& track)
    {
        //fTracks.insert(Tracks::value_type(track.GetTrackID(), track));
        fTracks.insert(track);
        return;
    }

    void KTMultiTrackEventData::AddTrack(const KTProcessedTrackData& track)
    {
        Nymph::KTDataPtr data;
        KTProcessedTrackData& newTrack = data->Of< KTProcessedTrackData >();
        newTrack = track;

        AllTrackData trackObject( data, newTrack );
        AddTrack( trackObject );
        return;
    }

    void KTMultiTrackEventData::AddTracks(TrackSetCItSet tracks)
    {
        AddTracks(tracks, fTotalEventSequences);
        fTotalEventSequences++;
        return;
    }

    void KTMultiTrackEventData::AddTracks(TrackSet tracks)
    {
        AddTracks(tracks, fTotalEventSequences);
        fTotalEventSequences++;
        return;
    }

    void KTMultiTrackEventData::AddTracks(TrackSetCItSet tracks, ssize_t eventSequenceID)
    {
        for ( TrackSetCItSet::iterator aTrack=tracks.begin(); aTrack != tracks.end(); ++aTrack )
        {
            AllTrackData aProcessedTrack = AllTrackData(**aTrack);
            aProcessedTrack.fProcTrack.SetEventSequenceID(eventSequenceID);
            AddTrack(aProcessedTrack);
        }
        return;
    }

    void KTMultiTrackEventData::AddTracks(TrackSet tracks, ssize_t eventSequenceID)
    {
        for ( TrackSet::iterator aTrack=tracks.begin(); aTrack != tracks.end(); ++aTrack )
        {
            AllTrackData aProcessedTrack = AllTrackData(*aTrack);
            aProcessedTrack.fProcTrack.SetEventSequenceID(eventSequenceID);
            AddTrack(aProcessedTrack);
        }
        return;
    }

    void KTMultiTrackEventData::ProcessTracks()
    {
        KTDEBUG(evlog, "Processing tracks");

        TrackSetCIt trackIt = fTracks.begin();

        fStartTimeInAcq = trackIt->fProcTrack.GetStartTimeInAcq();
        fStartTimeInRunC = trackIt->fProcTrack.GetStartTimeInRunC();
        fEndTimeInRunC = trackIt->fProcTrack.GetEndTimeInRunC();
        fTimeLength = trackIt->fProcTrack.GetTimeLength();
        fStartFrequency = trackIt->fProcTrack.GetStartFrequency();
        fEndFrequency = trackIt->fProcTrack.GetEndFrequency();
        fMinimumFrequency = std::min(fStartFrequency, fEndFrequency);
        fMaximumFrequency = std::max(fStartFrequency, fEndFrequency);
        fFrequencyWidth = trackIt->fProcTrack.GetFrequencyWidth();

        fStartTimeInRunCSigma = trackIt->fProcTrack.GetStartTimeInRunCSigma();
        fEndTimeInRunCSigma = trackIt->fProcTrack.GetEndTimeInRunCSigma();
        fTimeLengthSigma = trackIt->fProcTrack.GetTimeLengthSigma();
        fStartFrequencySigma = trackIt->fProcTrack.GetStartFrequencySigma();
        fEndFrequencySigma = trackIt->fProcTrack.GetEndFrequencySigma();
        fFrequencyWidthSigma = trackIt->fProcTrack.GetFrequencyWidthSigma();

        fFirstTrackID = trackIt->fProcTrack.GetTrackID();
        fFirstTrackTimeLength = trackIt->fProcTrack.GetTimeLength();
        fFirstTrackFrequencyWidth = trackIt->fProcTrack.GetFrequencyWidth();
        fFirstTrackSlope = trackIt->fProcTrack.GetSlope();
        fFirstTrackIntercept = trackIt->fProcTrack.GetIntercept();
        fFirstTrackTotalPower = trackIt->fProcTrack.GetTotalPower();

        for (++trackIt; trackIt != fTracks.end(); ++trackIt)
        {
            KTDEBUG(evlog, "Track " << trackIt->fProcTrack.GetTrackID());

            if (trackIt->fProcTrack.GetStartTimeInRunC() < fStartTimeInRunC)
            {
                fStartTimeInAcq = trackIt->fProcTrack.GetStartTimeInAcq();
                fStartTimeInRunC = trackIt->fProcTrack.GetStartTimeInRunC();
                fStartTimeInRunCSigma = trackIt->fProcTrack.GetStartTimeInRunCSigma();
                fStartFrequency = trackIt->fProcTrack.GetStartFrequency();
                fStartFrequencySigma = trackIt->fProcTrack.GetStartFrequencySigma();
                KTDEBUG(evlog, "Start time (freq) is now " << fStartTimeInRunC << "(" << fStartFrequency << ")");
                fFirstTrackID = trackIt->fProcTrack.GetTrackID();
                fFirstTrackTimeLength = trackIt->fProcTrack.GetTimeLength();
                fFirstTrackFrequencyWidth = trackIt->fProcTrack.GetFrequencyWidth();
                fFirstTrackSlope = trackIt->fProcTrack.GetSlope();
                fFirstTrackIntercept = trackIt->fProcTrack.GetIntercept();
                fFirstTrackTotalPower = trackIt->fProcTrack.GetTotalPower();
            }

            if (trackIt->fProcTrack.GetEndTimeInRunC() > fEndTimeInRunC)
            {
                fEndTimeInRunC = trackIt->fProcTrack.GetEndTimeInRunC();
                fEndTimeInRunCSigma = trackIt->fProcTrack.GetEndTimeInRunCSigma();
                fEndFrequency = trackIt->fProcTrack.GetEndFrequency();
                fEndFrequencySigma = trackIt->fProcTrack.GetEndFrequencySigma();
                KTDEBUG(evlog, "End time (freq) is now " << fEndTimeInRunC << "(" << fEndFrequency << ")");
            }

            double minFreq = std::min(trackIt->fProcTrack.GetStartFrequency(), trackIt->fProcTrack.GetEndFrequency());
            if (minFreq < fMinimumFrequency)
            {
                fMinimumFrequency = minFreq;
                KTDEBUG(evlog, "Minimum frequency is now " << fMinimumFrequency);
            }

            double maxFreq = std::max(trackIt->fProcTrack.GetStartFrequency(), trackIt->fProcTrack.GetEndFrequency());
            if (maxFreq > fMaximumFrequency)
            {
                fMaximumFrequency = maxFreq;
                KTDEBUG(evlog, "Maximum frequency is now " << fMaximumFrequency);
            }
        }

        fTimeLength = fEndTimeInRunC - fStartTimeInRunC;
        fTimeLengthSigma = sqrt(fEndTimeInRunCSigma * fEndTimeInRunCSigma + fStartTimeInRunCSigma * fStartTimeInRunCSigma);

        fFrequencyWidth = fMaximumFrequency - fMinimumFrequency;
        fFrequencyWidthSigma = sqrt(fMaximumFrequency * fEndFrequencySigma + fMinimumFrequency * fMinimumFrequency);

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

    MultiPeakTrackRef::MultiPeakTrackRef() :
            fTrackRefs(),
            fMeanStartTimeInRunC(0.),
            fSumStartTimeInRunC(0.),
            fMeanEndTimeInRunC(0.),
            fSumEndTimeInRunC(0.),
            fAcquisitionID(0),
            fUnknownEventTopology(false)
    {}

    bool MultiPeakTrackRef::InsertTrack(const TrackSetCIt& trackRef)
    {
        if (fTrackRefs.find(trackRef) != fTrackRefs.end())  return false;

        fTrackRefs.insert(trackRef);
        fSumStartTimeInRunC += trackRef->fProcTrack.GetStartTimeInRunC();
        fSumEndTimeInRunC += trackRef->fProcTrack.GetEndTimeInRunC();
        double currentSize = (double)fTrackRefs.size();
        fMeanStartTimeInRunC = fSumStartTimeInRunC / currentSize;
        fMeanEndTimeInRunC = fSumEndTimeInRunC / currentSize;
        return true;
    }

} /* namespace Katydid */
