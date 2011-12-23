/*
 * KTEvent.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEvent.hh"

#include "TH1.h"

#include <iostream>

ClassImp(Katydid::KTEvent);

namespace Katydid
{

    KTEvent::KTEvent() :
                fSampleRate(0.),
                fRecordLength(0.),
                fBinWidth(1.),
                fTimeStamp(),
                fFrameID(),
                fRecord()
    {
    }

    KTEvent::~KTEvent()
    {
    }
    /*
    UInt_t KTEvent::GetADCAtTime(Double_t time) const
    {
        return this->GetADCAtBin(TMath::Nint(time / fBinWidth));
    }

    UInt_t KTEvent::GetADCAtBin(Int_t bin) const
    {
        return fRecord[bin];
    }
    */
    TH1I* KTEvent::CreateAmplitudeDistributionHistogram() const
    {
        TH1I* hist = new TH1I("hRecord", "Event Record", 256, -0.5, 255.5);
        for (int iBin=0; iBin<fRecord.size(); iBin++)
        {
            hist->Fill((Double_t)(fRecord[iBin]));
        }
        hist->SetXTitle("ADC Bin");
        return hist;
    }

    const vector< UInt_t >& KTEvent::GetFrameID() const
    {
        return fFrameID;
    }

    const vector< UInt_t >& KTEvent::GetRecord() const
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

    const vector< UInt_t >& KTEvent::GetTimeStamp() const
    {
        return fTimeStamp;
    }

    Double_t KTEvent::GetBinWidth() const
    {
        return fBinWidth;
    }

    unsigned int KTEvent::GetRecordSize() const
    {
        return (unsigned int)fRecord.size();
    }

    UInt_t KTEvent::GetRecordAt(unsigned int iPoint) const
    {
        return fRecord[iPoint];
    }

    UInt_t KTEvent::GetRecordAtTime(Double_t time) const
    {
        return this->GetRecordAt((unsigned int)(TMath::Nint(TMath::Max(0., time) / fBinWidth)));
    }

    void KTEvent::SetFrameID(const vector< UInt_t >& frameID)
    {
        this->fFrameID = frameID;
    }

    void KTEvent::SetRecord(const vector< UInt_t >& record)
    {
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

    void KTEvent::SetTimeStamp(const vector< UInt_t >& timeStamp)
    {
        this->fTimeStamp = timeStamp;
    }

} /* namespace Katydid */
