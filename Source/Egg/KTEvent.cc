/*
 * KTEvent.cc
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#include "KTEvent.hh"

#include "KTArrayUC.hh"

#include <cstring>

ClassImp(Katydid::KTEvent);

namespace Katydid
{

    KTEvent::KTEvent() :
                fTimeStampSize(0),
                fFrameIDSize(0),
                fRecordSize(0),
                fEventSize(0),
                fSampleRate(0),
                fSampleLength(0),
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

} /* namespace Katydid */
