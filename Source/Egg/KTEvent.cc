/*
 * KTEvent.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEvent.hh"

#include "KTArrayUC.hh"

#include "TH1.h"
#include "TMath.h"

ClassImp(Katydid::KTEvent);

namespace Katydid
{

    KTEvent::KTEvent() :
                fTimeStampSize(0),
                fFrameIDSize(0),
                fRecordSize(1),
                fEventSize(0),
                fSampleRate(0),
                fSampleLength(0),
                fTimeStamp(NULL),
                fFrameID(NULL),
                fRecord(NULL),
                fHertzPerSampleRateUnit(1.),
                fSecondsPerSampleLengthUnit(1.),
                fBinWidth(1.)
    {
    }

    KTEvent::~KTEvent()
    {
        delete fTimeStamp;
        delete fFrameID;
        delete fRecord;
    }

    UInt_t KTEvent::GetADCAtTime(Double_t time) const
    {
        return this->GetADCAtBin(TMath::Nint(time / fBinWidth));
    }

    UInt_t KTEvent::GetADCAtBin(Int_t bin) const
    {
        return (UInt_t)fRecord->At(bin);
    }

    TH1I* KTEvent::CreateRecordHistogram() const
    {
        TH1I* hist = new TH1I("hRecord", "Event Record", 256, -0.5, 255.5);
        for (Int_t iBin=0; iBin<fRecord->GetSize(); iBin++)
        {
            hist->Fill((Double_t)fRecord->At(iBin));
        }
        hist->SetXTitle("ADC Bin");
        return hist;
    }

    Int_t KTEvent::GetEventSize() const
    {
        return fEventSize;
    }

    const KTArrayUC* KTEvent::GetFrameID() const
    {
        return fFrameID;
    }

    Int_t KTEvent::GetFrameIDSize() const
    {
        return fFrameIDSize;
    }

    const KTArrayUC* KTEvent::GetRecord() const
    {
        return fRecord;
    }

    Int_t KTEvent::GetRecordSize() const
    {
        return fRecordSize;
    }

    Int_t KTEvent::GetSampleLength() const
    {
        return fSampleLength;
    }

    Int_t KTEvent::GetSampleRate() const
    {
        return fSampleRate;
    }

    const KTArrayUC* KTEvent::GetTimeStamp() const
    {
        return fTimeStamp;
    }

    Int_t KTEvent::GetTimeStampSize() const
    {
        return fTimeStampSize;
    }

    Double_t KTEvent::GetHertzPerSampleRateUnit() const
    {
        return fHertzPerSampleRateUnit;
    }

    Double_t KTEvent::GetSecondsPerSampleLengthUnit() const
    {
        return fSecondsPerSampleLengthUnit;
    }

    Double_t KTEvent::GetBinWidth() const
    {
        return fBinWidth;
    }

    void KTEvent::SetEventSize(Int_t eventSize)
    {
        this->fEventSize = eventSize;
    }

    void KTEvent::SetFrameID(KTArrayUC* frameID)
    {
        delete fFrameID;
        this->fFrameID = frameID;
    }

    void KTEvent::SetFrameIDSize(Int_t frameIDSize)
    {
        this->fFrameIDSize = frameIDSize;
    }

    void KTEvent::SetRecord(KTArrayUC* record)
    {
        delete fFrameID;
        this->fRecord = record;
    }

    void KTEvent::SetRecordSize(Int_t recordSize)
    {
        this->fRecordSize = recordSize;
    }

    void KTEvent::SetSampleLength(Int_t sampleLength)
    {
        this->fSampleLength = sampleLength;
    }

    void KTEvent::SetSampleRate(Int_t sampleRate)
    {
        this->fSampleRate = sampleRate;
        this->fBinWidth = fHertzPerSampleRateUnit / sampleRate;
    }

    void KTEvent::SetTimeStamp(KTArrayUC* timeStamp)
    {
        delete fFrameID;
        this->fTimeStamp = timeStamp;
    }

    void KTEvent::SetTimeStampSize(Int_t timeStampSize)
    {
        this->fTimeStampSize = timeStampSize;
    }

    void KTEvent::SetHertzPerSampleRateUnit(Double_t hpsru)
    {
        fHertzPerSampleRateUnit = hpsru;
        return;
    }

    void KTEvent::SetSecondsPerSampleLengthUnit(Double_t spslu)
    {
        fSecondsPerSampleLengthUnit = spslu;
        return;
    }

} /* namespace Katydid */
