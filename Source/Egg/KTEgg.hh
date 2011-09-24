/*
 * KTEgg.hh
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */

#ifndef KTEGG_HH_
#define KTEGG_HH_

#include "Rtypes.h"

#include <fstream>
using std::ifstream;
#include <string>
using std::string;

class KTArrayUC;
class TArrayC;

namespace Katydid
{
    class KTEvent;

    class KTEgg
    {
        public:
            KTEgg();
            virtual ~KTEgg();

            Bool_t BreakEgg();
            Bool_t ParseEggHeader();
            KTEvent* HatchNextEvent();

            const string& GetFileName() const;
            const ifstream& GetEggStream() const;
            UInt_t GetHeaderSize() const;
            const string& GetHeader() const;
            const string& GetPrelude() const;

            Int_t GetEventSize() const;
            Int_t GetFrameIDSize() const;
            Int_t GetRecordSize() const;
            Double_t GetApproxRecordLength() const;
            Double_t GetSampleRate() const;
            Int_t GetTimeStampSize() const;

            Double_t GetHertzPerSampleRateUnit() const;
            Double_t GetSecondsPerApproxRecordLengthUnit() const;


            void SetFileName(const string& fileName);
            void SetHeaderSize(UInt_t size);
            void SetHeader(const string& header);
            void SetPrelude(const string& prelude);

            void SetEventSize(Int_t size);
            void SetFrameIDSize(Int_t size);
            void SetRecordSize(Int_t size);
            void SetApproxRecordLength(Double_t length);
            void SetSampleRate(Double_t rate);
            void SetTimeStampSize(Int_t size);

            void SetHertzPerSampleRateUnit(Double_t hpsru);
            void SetSecondsPerApproxRecordLengthUnit(Double_t spslu);

        private:
            string fFileName;
            ifstream fEggStream;
            string fPrelude;
            UInt_t fHeaderSize;
            string fHeader;

            Int_t fTimeStampSize;
            Int_t fFrameIDSize;
            Int_t fRecordSize;
            Int_t fEventSize;

            Double_t fApproxRecordLength;
            Double_t fSampleRate;

            Double_t fHertzPerSampleRateUnit;
            Double_t fSecondsPerApproxRecordLengthUnit;


            static const ifstream::pos_type sPreludeSize;  // the prelude size is currently restricted to eight bytes

            ClassDef(KTEgg, 1);

    };

} /* namespace Katydid */

#endif /* KTEGG_HH_ */
