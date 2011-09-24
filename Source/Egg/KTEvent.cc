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
                fSampleRate(0.),
                fRecordLength(0.),
                fBinWidth(1.),
                fTimeStamp(NULL),
                fFrameID(NULL),
                fRecord(NULL)
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
        return (UInt_t)(*fRecord)[bin];
    }

    TH1I* KTEvent::CreateAmplitudeDistributionHistogram() const
    {
        TH1I* hist = new TH1I("hRecord", "Event Record", 256, -0.5, 255.5);
        for (Int_t iBin=0; iBin<fRecord->GetSize(); iBin++)
        {
            hist->Fill((Double_t)(*fRecord)[iBin]);
        }
        hist->SetXTitle("ADC Bin");
        return hist;
    }

    const KTArrayUC* KTEvent::GetFrameID() const
    {
        return fFrameID;
    }

    const KTArrayUC* KTEvent::GetRecord() const
    {
        return fRecord;
    }

    Double_t KTEvent::GetRecordLength() const
    {
        return fRecordLength;
    }

    Double_t KTEvent::GetSampleRate() const
    {
        return fSampleRate;
    }

    const KTArrayUC* KTEvent::GetTimeStamp() const
    {
        return fTimeStamp;
    }

    Double_t KTEvent::GetBinWidth() const
    {
        return fBinWidth;
    }

    void KTEvent::SetFrameID(KTArrayUC* frameID)
    {
        delete fFrameID;
        this->fFrameID = frameID;
    }

    void KTEvent::SetRecord(KTArrayUC* record)
    {
        delete fFrameID;
        this->fRecord = record;
    }

    void KTEvent::SetRecordLength(Double_t recordLength)
    {
        this->fRecordLength = recordLength;
    }

    void KTEvent::SetSampleRate(Double_t sampleRate)
    {
        this->fSampleRate = sampleRate;
    }

    void KTEvent::SetBinWidth(Double_t binWidth)
    {
        this->fBinWidth = binWidth;
    }

    void KTEvent::SetTimeStamp(KTArrayUC* timeStamp)
    {
        delete fFrameID;
        this->fTimeStamp = timeStamp;
    }

} /* namespace Katydid */
