/*
 * KTEvent.hh
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */
/**
 @file KTEvent.hh
 @brief Contains KTEvent
 @details Contains the information from a single Egg event in the form of a 1-D vector of unsigned integers.
 The data are the time series of the event.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTEVENT_HH_
#define KTEVENT_HH_

#include "TMath.h"

#include <fstream>
using std::ifstream;
#include <string>
using std::string;
#include <vector>
using std::vector;

class TH1I;

namespace Katydid
{
    class KTEvent
    {
        public:
            KTEvent();
            virtual ~KTEvent();

            virtual TH1I* CreateEventHistogram() const;
            virtual TH1I* CreateAmplitudeDistributionHistogram() const;

            Double_t GetBinWidth() const;
            const vector< UInt_t >& GetFrameID() const;
            const vector< UInt_t >& GetRecord() const;
            Double_t GetRecordLength() const;
            Double_t GetSampleRate() const;
            const vector< UInt_t >& GetTimeStamp() const;

            unsigned int GetRecordSize() const;
            UInt_t GetRecordAt(unsigned int iBin) const;
            template< typename XType >
            XType GetRecordAt(unsigned int iBin) const;
            UInt_t GetRecordAtTime(Double_t time) const; /// time is in seconds and >= 0
            template< typename XType >
            XType GetRecordAtTime(Double_t time) const; /// time is in seconds and >= 0

            void SetBinWidth(Double_t binWidth);
            void SetFrameID(const vector< UInt_t >& frameID);
            void SetRecord(const vector< UInt_t >& record);
            void SetRecordLength(Double_t recordLength);
            void SetSampleRate(Double_t sampleRate);
            void SetTimeStamp(const vector< UInt_t >& timeStamp);

        private:
            Double_t fSampleRate; // in Hz
            Double_t fRecordLength; // in sec
            Double_t fBinWidth; // in sec
            vector< UInt_t > fTimeStamp;
            vector< UInt_t > fFrameID;
            vector< UInt_t > fRecord;

            ClassDef(KTEvent, 1);

    };

    template< typename XType >
    XType KTEvent::GetRecordAt(unsigned int iPoint) const
    {
        return (XType)GetRecordAt(iPoint);
    }

    template< typename XType >
    XType KTEvent::GetRecordAtTime(Double_t time) const
    {
        return this->GetRecordAt< XType >((unsigned int)(TMath::Nint(TMath::Max(0., time) / fBinWidth)));
    }


} /* namespace Katydid */

#endif /* KTEVENT_HH_ */
