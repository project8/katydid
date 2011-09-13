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

namespace Katydid
{
    class KTEvent
    {
        public:
            KTEvent();
            virtual ~KTEvent();

            Int_t GetEventSize() const;
            const KTArrayUC* GetFrameID() const;
            Int_t GetFrameIDSize() const;
            const KTArrayUC* GetRecord() const;
            Int_t GetRecordSize() const;
            Int_t GetSampleLength() const;
            Int_t GetSampleRate() const;
            const KTArrayUC* GetTimeStamp() const;
            Int_t GetTimeStampSize() const;

            void SetEventSize(Int_t KTEventSize);
            void SetFrameID(KTArrayUC* frameID);
            void SetFrameIDSize(Int_t frameIDSize);
            void SetRecord(KTArrayUC* record);
            void SetRecordSize(Int_t recordSize);
            void SetSampleLength(Int_t sampleLength);
            void SetSampleRate(Int_t sampleRate);
            void SetTimeStamp(KTArrayUC* timeStamp);
            void SetTimeStampSize(Int_t timeStampSize);

        private:
            Int_t fTimeStampSize;
            Int_t fFrameIDSize;
            Int_t fRecordSize;
            Int_t fEventSize;
            Int_t fSampleRate; //in MHz
            Int_t fSampleLength; //in sec
            KTArrayUC* fTimeStamp;
            KTArrayUC* fFrameID;
            KTArrayUC* fRecord;

            ClassDef(KTEvent, 1);

    };

} /* namespace Katydid */

#endif /* KTEVENT_HH_ */
