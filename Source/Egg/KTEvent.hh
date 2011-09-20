/*
 * KTEvent.hh
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#ifndef KTEVENT_HH_
#define KTEVENT_HH_

#include "Rtypes.h"

#include <fstream>
using std::ifstream;
#include <string>
using std::string;

class KTArrayUC;
class TH1I;

namespace Katydid
{
    class KTEvent
    {
        public:
            KTEvent();
            virtual ~KTEvent();

            // time is expected in seconds
            UInt_t GetADCAtTime(Double_t time) const;
            UInt_t GetADCAtBin(Int_t bin) const;

            virtual TH1I* CreateRecordHistogram() const;

            Int_t GetEventSize() const;
            const KTArrayUC* GetFrameID() const;
            Int_t GetFrameIDSize() const;
            const KTArrayUC* GetRecord() const;
            Int_t GetRecordSize() const;
            Int_t GetSampleLength() const;
            Int_t GetSampleRate() const;
            const KTArrayUC* GetTimeStamp() const;
            Int_t GetTimeStampSize() const;

            Double_t GetHertzPerSampleRateUnit() const;
            Double_t GetSecondsPerSampleLengthUnit() const;
            Double_t GetBinWidth() const;

            void SetEventSize(Int_t KTEventSize);
            void SetFrameID(KTArrayUC* frameID);
            void SetFrameIDSize(Int_t frameIDSize);
            void SetRecord(KTArrayUC* record);
            void SetRecordSize(Int_t recordSize);
            void SetSampleLength(Int_t sampleLength);
            void SetSampleRate(Int_t sampleRate);
            void SetTimeStamp(KTArrayUC* timeStamp);
            void SetTimeStampSize(Int_t timeStampSize);

            void SetHertzPerSampleRateUnit(Double_t hpsru);
            void SetSecondsPerSampleLengthUnit(Double_t spslu);

        private:
            Int_t fTimeStampSize;
            Int_t fFrameIDSize;
            Int_t fRecordSize;
            Int_t fEventSize;
            Int_t fSampleRate; //in MHz
            Int_t fSampleLength; //in msec
            KTArrayUC* fTimeStamp;
            KTArrayUC* fFrameID;
            KTArrayUC* fRecord;

            Double_t fHertzPerSampleRateUnit;
            Double_t fSecondsPerSampleLengthUnit;
            Double_t fBinWidth; // in sec

            ClassDef(KTEvent, 1);

    };

} /* namespace Katydid */

#endif /* KTEVENT_HH_ */
