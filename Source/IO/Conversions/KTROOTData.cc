/*
 * KTROOTData.cc
 *
 *  Created on: Aug 4, 2014
 *      Author: nsoblath
 */

#include "KTROOTData.hh"

#include "KTMultiTrackEventData.hh"
#include "KTProcessedTrackData.hh"

#include "logger.hh"

#include "TClonesArray.h"

#include <iostream>

ClassImp(Katydid::TProcessedTrackData);
ClassImp(Katydid::TMultiTrackEventData);

namespace Katydid
{
    //***********************
    // TProcessedTrackData
    //***********************

    TProcessedTrackData::TProcessedTrackData() :
            TObject(),
            fComponent(0), fTrackID(0), fEventSequenceID(-1), fIsCut(false),
            fAcquisitionID(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fFrequencyWidth(0.),
            fSlope(0.), fIntercept(0.), fTotalPower(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fSlopeSigma(0.), fInterceptSigma(0.), fTotalPowerSigma(0.)
    {}

    TProcessedTrackData::TProcessedTrackData(const KTProcessedTrackData& orig) :
            TObject(),
            fComponent(0), fTrackID(0), fEventSequenceID(-1), fIsCut(false),
            fAcquisitionID(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fFrequencyWidth(0.),
            fSlope(0.), fIntercept(0.), fTotalPower(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fSlopeSigma(0.), fInterceptSigma(0.), fTotalPowerSigma(0.)
    {
        Load(orig);
    }

    TProcessedTrackData::TProcessedTrackData(const TProcessedTrackData& orig) :
            TObject(orig),
            fComponent(orig.fComponent), fTrackID(orig.fTrackID), fEventSequenceID(orig.fEventSequenceID), fIsCut(orig.fIsCut),
            fAcquisitionID(orig.fAcquisitionID),
            fStartTimeInRunC(orig.fStartTimeInRunC), fStartTimeInAcq(orig.fStartTimeInAcq), fEndTimeInRunC(orig.fEndTimeInRunC),fTimeLength(orig.fTimeLength),
            fStartFrequency(orig.fStartFrequency), fEndFrequency(orig.fEndFrequency), fFrequencyWidth(orig.fFrequencyWidth),
            fSlope(orig.fSlope), fIntercept(orig.fIntercept), fTotalPower(orig.fTotalPower),
            fStartTimeInRunCSigma(orig.fStartTimeInRunCSigma), fEndTimeInRunCSigma(orig.fEndTimeInRunCSigma), fTimeLengthSigma(orig.fTimeLengthSigma),
            fStartFrequencySigma(orig.fStartFrequencySigma), fEndFrequencySigma(orig.fEndFrequencySigma), fFrequencyWidthSigma(orig.fFrequencyWidthSigma),
            fSlopeSigma(orig.fSlopeSigma), fInterceptSigma(orig.fInterceptSigma), fTotalPowerSigma(orig.fTotalPowerSigma)
    {}

    TProcessedTrackData::~TProcessedTrackData()
    {}

    TObject* TProcessedTrackData::Clone(const char* newname)
    {
        TProcessedTrackData* newData = new TProcessedTrackData(*this);
        return newData;
    }

    TProcessedTrackData& TProcessedTrackData::operator=(const TProcessedTrackData& rhs)
    {
        fComponent = rhs.fComponent;fTrackID = rhs.fTrackID; fEventSequenceID = rhs.fEventSequenceID; fIsCut = rhs.fIsCut;
        fAcquisitionID = rhs.fAcquisitionID;
        fStartTimeInRunC = rhs.fStartTimeInRunC; fStartTimeInAcq = rhs.fStartTimeInAcq; fEndTimeInRunC = rhs.fEndTimeInRunC;fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency; fEndFrequency = rhs.fEndFrequency; fFrequencyWidth = rhs.fFrequencyWidth;
        fSlope = rhs.fSlope; fIntercept = rhs.fIntercept; fTotalPower = rhs.fTotalPower;
        fStartTimeInRunCSigma = rhs.fStartTimeInRunCSigma; fEndTimeInRunCSigma = rhs.fEndTimeInRunCSigma; fTimeLengthSigma = rhs.fTimeLengthSigma;
        fStartFrequencySigma = rhs.fStartFrequencySigma; fEndFrequencySigma = rhs.fEndFrequencySigma; fFrequencyWidthSigma = rhs.fFrequencyWidthSigma;
        fSlopeSigma = rhs.fSlopeSigma; fInterceptSigma = rhs.fInterceptSigma; fTotalPowerSigma = rhs.fTotalPowerSigma;
        return *this;
    }

    void TProcessedTrackData::Load(const KTProcessedTrackData& data)
    {
        fComponent = data.GetComponent();fTrackID = data.GetTrackID(); fEventSequenceID = data.GetEventSequenceID(); fIsCut = data.GetIsCut();
        fAcquisitionID = data.GetAcquisitionID();
        fStartTimeInRunC = data.GetStartTimeInRunC(); fStartTimeInAcq = data.GetStartTimeInAcq(); fEndTimeInRunC = data.GetEndTimeInRunC();fTimeLength = data.GetTimeLength();
        fStartFrequency = data.GetStartFrequency(); fEndFrequency = data.GetEndFrequency(); fFrequencyWidth = data.GetFrequencyWidth();
        fSlope = data.GetSlope(); fIntercept = data.GetIntercept(); fTotalPower = data.GetTotalPower();
        fStartTimeInRunCSigma = data.GetStartTimeInRunCSigma(); fEndTimeInRunCSigma = data.GetEndTimeInRunCSigma(); fTimeLengthSigma = data.GetTimeLengthSigma();
        fStartFrequencySigma = data.GetStartFrequencySigma(); fEndFrequencySigma = data.GetEndFrequencySigma(); fFrequencyWidthSigma = data.GetFrequencyWidthSigma();
        fSlopeSigma = data.GetSlopeSigma(); fInterceptSigma = data.GetInterceptSigma(); fTotalPowerSigma = data.GetTotalPowerSigma();
        return;
    }
    void TProcessedTrackData::Unload(KTProcessedTrackData& data) const
    {
        data.SetComponent(fComponent); data.SetTrackID(fTrackID); data.SetEventSequenceID(fEventSequenceID); data.SetIsCut(fIsCut);
        data.SetAcquisitionID(fAcquisitionID);
        data.SetStartTimeInRunC(fStartTimeInRunC); data.SetStartTimeInAcq(fStartTimeInAcq); data.SetEndTimeInRunC(fEndTimeInRunC); data.SetTimeLength(fTimeLength);
        data.SetStartFrequency(fStartFrequency); data.SetEndFrequency(fEndFrequency); data.SetFrequencyWidth(fFrequencyWidth);
        data.SetSlope(fSlope); data.SetIntercept(fIntercept); data.SetTotalPower(fTotalPower);
        data.SetStartTimeInRunCSigma(fStartTimeInRunCSigma); data.SetEndTimeInRunCSigma(fEndTimeInRunCSigma); data.SetTimeLengthSigma(fTimeLengthSigma);
        data.SetStartFrequencySigma(fStartFrequencySigma); data.SetEndFrequencySigma(fEndFrequencySigma); data.SetFrequencyWidthSigma(fFrequencyWidthSigma);
        data.SetSlopeSigma(fSlopeSigma); data.SetInterceptSigma(fInterceptSigma); data.SetTotalPowerSigma(fTotalPowerSigma);
        return;
    }



    //************************
    // TMultiTrackEventData
    //************************

    TMultiTrackEventData::TMultiTrackEventData() :
            fComponent(0), fEventID(0),
            fAcquisitionID(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fMinimumFrequency(0.), fMaximumFrequency(0.), fFrequencyWidth(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fFirstTrackTimeLength(0.), fFirstTrackFrequencyWidth(0.), fFirstTrackSlope(0.), fFirstTrackIntercept(0.), fFirstTrackTotalPower(0.),
            fUnknownEventTopology(false)
    {
        // this cannot be initialized in the initializer list because ROOT
        fTracks = new TClonesArray("Katydid::TProcessedTrackData", 20);
    }

    TMultiTrackEventData::TMultiTrackEventData(const TMultiTrackEventData& orig) :
            fComponent(orig.fComponent), fEventID(orig.fEventID),
            fAcquisitionID(orig.fAcquisitionID),
            fStartTimeInRunC(orig.fStartTimeInRunC), fStartTimeInAcq(orig.fStartTimeInAcq), fEndTimeInRunC(orig.fEndTimeInRunC),fTimeLength(orig.fTimeLength),
            fStartFrequency(orig.fStartFrequency), fEndFrequency(orig.fEndFrequency), fMinimumFrequency(orig.fMaximumFrequency), fMaximumFrequency(orig.fMinimumFrequency), fFrequencyWidth(orig.fFrequencyWidth),
            fStartTimeInRunCSigma(orig.fStartTimeInRunCSigma), fEndTimeInRunCSigma(orig.fEndTimeInRunCSigma), fTimeLengthSigma(orig.fTimeLengthSigma),
            fStartFrequencySigma(orig.fStartFrequencySigma), fEndFrequencySigma(orig.fEndFrequencySigma), fFrequencyWidthSigma(orig.fFrequencyWidthSigma),
            fFirstTrackTimeLength(orig.fFirstTrackTimeLength), fFirstTrackFrequencyWidth(orig.fFirstTrackFrequencyWidth), fFirstTrackSlope(orig.fFirstTrackSlope), fFirstTrackIntercept(orig.fFirstTrackIntercept), fFirstTrackTotalPower(orig.fFirstTrackTotalPower),
            fUnknownEventTopology(orig.fUnknownEventTopology)
    {
        // this cannot be initialized in the initializer list because ROOT
        fTracks = new TClonesArray(*orig.fTracks);
    }

    TMultiTrackEventData::TMultiTrackEventData(const KTMultiTrackEventData& orig) :
            fComponent(0), fEventID(0),
            fAcquisitionID(0),
            fStartTimeInRunC(0.), fStartTimeInAcq(0.), fEndTimeInRunC(0.),fTimeLength(0.),
            fStartFrequency(0.), fEndFrequency(0.), fMinimumFrequency(0.), fMaximumFrequency(0.), fFrequencyWidth(0.),
            fStartTimeInRunCSigma(0.), fEndTimeInRunCSigma(0.), fTimeLengthSigma(0.),
            fStartFrequencySigma(0.), fEndFrequencySigma(0.), fFrequencyWidthSigma(0.),
            fFirstTrackTimeLength(0.), fFirstTrackFrequencyWidth(0.), fFirstTrackSlope(0.), fFirstTrackIntercept(0.), fFirstTrackTotalPower(0.),
            fUnknownEventTopology(false)
    {
        // this cannot be initialized in the initializer list because ROOT
        fTracks = new TClonesArray("Katydid::TProcessedTrackData", 20);
        Load(orig);
    }

    TMultiTrackEventData::~TMultiTrackEventData()
    {
        fTracks->Clear();
    }

    TObject* TMultiTrackEventData::Clone(const char* newname)
    {
        TMultiTrackEventData* newData = new TMultiTrackEventData(*this);
        return newData;
    }

    TMultiTrackEventData& TMultiTrackEventData::operator=(const TMultiTrackEventData& rhs)
    {
        fComponent = rhs.fComponent;fEventID = rhs.fEventID;
        fAcquisitionID = rhs.fAcquisitionID;
        fStartTimeInRunC = rhs.fStartTimeInRunC; fStartTimeInAcq = rhs.fStartTimeInAcq; fEndTimeInRunC = rhs.fEndTimeInRunC;fTimeLength = rhs.fTimeLength;
        fStartFrequency = rhs.fStartFrequency; fEndFrequency = rhs.fEndFrequency; fMinimumFrequency = rhs.fMinimumFrequency; fMaximumFrequency = rhs.fMaximumFrequency; fFrequencyWidth = rhs.fFrequencyWidth;
        fStartTimeInRunCSigma = rhs.fStartTimeInRunCSigma; fEndTimeInRunCSigma = rhs.fEndTimeInRunCSigma; fTimeLengthSigma = rhs.fTimeLengthSigma;
        fStartFrequencySigma = rhs.fStartFrequencySigma; fEndFrequencySigma = rhs.fEndFrequencySigma; fFrequencyWidthSigma = rhs.fFrequencyWidthSigma;
        fFirstTrackTimeLength = rhs.fFirstTrackTimeLength; fFirstTrackFrequencyWidth = rhs.fFirstTrackFrequencyWidth; fFirstTrackSlope = rhs.fFirstTrackSlope; fFirstTrackIntercept = rhs.fFirstTrackIntercept; fFirstTrackTotalPower = rhs.fFirstTrackTotalPower;
        fUnknownEventTopology = rhs.fUnknownEventTopology;
        fTracks->Clear(); (*fTracks) = *(rhs.fTracks);
        return *this;
    }

    void TMultiTrackEventData::Load(const KTMultiTrackEventData& data)
    {
        fComponent = data.GetComponent();fEventID = data.GetEventID();
        fAcquisitionID = data.GetAcquisitionID();
        fStartTimeInRunC = data.GetStartTimeInRunC(); fStartTimeInAcq = data.GetStartTimeInAcq(); fEndTimeInRunC = data.GetEndTimeInRunC();fTimeLength = data.GetTimeLength();
        fStartFrequency = data.GetStartFrequency(); fEndFrequency = data.GetEndFrequency(); fMinimumFrequency = data.GetMinimumFrequency(); fMaximumFrequency = data.GetMaximumFrequency(); fFrequencyWidth = data.GetFrequencyWidth();
        fStartTimeInRunCSigma = data.GetStartTimeInRunCSigma(); fEndTimeInRunCSigma = data.GetEndTimeInRunCSigma(); fTimeLengthSigma = data.GetTimeLengthSigma();
        fStartFrequencySigma = data.GetStartFrequencySigma(); fEndFrequencySigma = data.GetEndFrequencySigma(); fFrequencyWidthSigma = data.GetFrequencyWidthSigma();
        fFirstTrackTimeLength = data.GetFirstTrackTimeLength(); fFirstTrackFrequencyWidth = data.GetFirstTrackFrequencyWidth(); fFirstTrackSlope = data.GetFirstTrackSlope(); fFirstTrackIntercept = data.GetFirstTrackIntercept(); fFirstTrackTotalPower = data.GetFirstTrackTotalPower();
        fUnknownEventTopology = data.GetUnknownEventTopology();
        Int_t nTracks = (Int_t)data.GetNTracks();
        fTracks->Clear(); fTracks->Expand(nTracks);
        Int_t iTrack = 0;
        for (TrackSetCIt trIt = data.GetTracksBegin(); trIt != data.GetTracksEnd(); ++trIt)
        {
            TProcessedTrackData* track = new((*fTracks)[iTrack]) TProcessedTrackData(*trIt);
            ++iTrack;
        }
        return;
    }
    void TMultiTrackEventData::Unload(KTMultiTrackEventData& data) const
    {
        data.ClearTracks(); // do this first, since it clears some of the member variables other than just fTracks
        data.SetComponent(fComponent); data.SetEventID(fEventID);
        data.SetAcquisitionID(fAcquisitionID);
        data.SetStartTimeInRunC(fStartTimeInRunC); data.SetStartTimeInAcq(fStartTimeInAcq); data.SetEndTimeInRunC(fEndTimeInRunC); data.SetTimeLength(fTimeLength);
        data.SetStartFrequency(fStartFrequency); data.SetEndFrequency(fEndFrequency); data.SetMinimumFrequency(fMinimumFrequency); data.SetMaximumFrequency(fMaximumFrequency); data.SetFrequencyWidth(fFrequencyWidth);
        data.SetStartTimeInRunCSigma(fStartTimeInRunCSigma); data.SetEndTimeInRunCSigma(fEndTimeInRunCSigma); data.SetTimeLengthSigma(fTimeLengthSigma);
        data.SetStartFrequencySigma(fStartFrequencySigma); data.SetEndFrequencySigma(fEndFrequencySigma); data.SetFrequencyWidthSigma(fFrequencyWidthSigma);
        data.SetFirstTrackTimeLength(fFirstTrackTimeLength); data.SetFirstTrackFrequencyWidth(fFirstTrackFrequencyWidth); data.SetFirstTrackSlope(fFirstTrackSlope); data.SetFirstTrackIntercept(fFirstTrackIntercept); data.SetFirstTrackTotalPower(fFirstTrackTotalPower);
        data.SetUnknownEventTopology(fUnknownEventTopology);
        Int_t nTracks = fTracks->GetSize();
        KTProcessedTrackData track;
        for (Int_t iTrack = 0; iTrack < nTracks; ++iTrack)
        {
            ((TProcessedTrackData*)((*fTracks)[iTrack]))->Unload(track);
            data.AddTrack(track);
        }
        return;
    }

}
