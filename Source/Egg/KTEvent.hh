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

            virtual TH1I* CreateAmplitudeDistributionHistogram() const;

            Double_t GetBinWidth() const;
            const KTArrayUC* GetFrameID() const;
            const KTArrayUC* GetRecord() const;
            Double_t GetRecordLength() const;
            Double_t GetSampleRate() const;
            const KTArrayUC* GetTimeStamp() const;

            void SetBinWidth(Double_t binWidth);
            void SetFrameID(KTArrayUC* frameID);
            void SetRecord(KTArrayUC* record);
            void SetRecordLength(Double_t recordLength);
            void SetSampleRate(Double_t sampleRate);
            void SetTimeStamp(KTArrayUC* timeStamp);

        private:
            Double_t fSampleRate; // in Hz
            Double_t fRecordLength; // in sec
            Double_t fBinWidth; // in sec
            KTArrayUC* fTimeStamp;
            KTArrayUC* fFrameID;
            KTArrayUC* fRecord;

            ClassDef(KTEvent, 1);

    };

} /* namespace Katydid */

#endif /* KTEVENT_HH_ */
