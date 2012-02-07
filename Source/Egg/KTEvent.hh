/*
 * KTEvent.hh
 *
 *  Created on: Sep 9, 2011
 *      Author: nsoblath
 */
/**
 @file KTEvent.hh
 @brief Contains KTEvent
 @details Contains the information from a single Egg event in the form of a 1-D std::vector of unsigned integers.
 The data are the time series of the event.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTEVENT_HH_
#define KTEVENT_HH_

#include <algorithm>
#include <string>
#include <vector>

#ifdef ROOT_FOUND
class TH1I;
#endif

namespace Katydid
{
    class KTEvent
    {
        public:
            KTEvent();
            virtual ~KTEvent();

#ifdef ROOT_FOUND
            virtual TH1I* CreateEventHistogram() const;
            virtual TH1I* CreateAmplitudeDistributionHistogram() const;
#endif

            double GetBinWidth() const;
            const std::vector< unsigned >& GetFrameID() const;
            const std::vector< unsigned >& GetRecord() const;
            double GetRecordLength() const;
            double GetSampleRate() const;
            const std::vector< unsigned >& GetTimeStamp() const;

            unsigned int GetRecordSize() const;
            unsigned GetRecordAt(unsigned int iBin) const;
            template< typename XType >
            XType GetRecordAt(unsigned int iBin) const;
            unsigned GetRecordAtTime(double time) const; /// time is in seconds and >= 0
            template< typename XType >
            XType GetRecordAtTime(double time) const; /// time is in seconds and >= 0

            void SetBinWidth(double binWidth);
            void SetFrameID(const std::vector< unsigned >& frameID);
            void SetRecord(const std::vector< unsigned >& record);
            void SetRecordLength(double recordLength);
            void SetSampleRate(double sampleRate);
            void SetTimeStamp(const std::vector< unsigned >& timeStamp);

        private:
            double fSampleRate; // in Hz
            double fRecordLength; // in sec
            double fBinWidth; // in sec
            std::vector< unsigned > fTimeStamp;
            std::vector< unsigned > fFrameID;
            std::vector< unsigned > fRecord;

        private:
            /// Round to nearest integer. Rounds half integers to the nearest even integer.
            /// Based on ROOT's TMath::Nint(Double_t)
            int nint(double x) const;

    };


    template< typename XType >
    XType KTEvent::GetRecordAt(unsigned int iPoint) const
    {
        return (XType)GetRecordAt(iPoint);
    }

    template< typename XType >
    XType KTEvent::GetRecordAtTime(double time) const
    {
        return this->GetRecordAt< XType >((unsigned int)(nint(std::max(0., time) / fBinWidth)));
    }


    inline const std::vector< unsigned >& KTEvent::GetFrameID() const
    {
        return fFrameID;
    }

    inline const std::vector< unsigned >& KTEvent::GetRecord() const
    {
        return fRecord;
    }

    inline double KTEvent::GetRecordLength() const
    {
        return fRecordLength;
    }

    inline double KTEvent::GetSampleRate() const
    {
        return fSampleRate;
    }

    inline const std::vector< unsigned >& KTEvent::GetTimeStamp() const
    {
        return fTimeStamp;
    }

    inline double KTEvent::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline unsigned KTEvent::GetRecordSize() const
    {
        return (unsigned)fRecord.size();
    }

    inline unsigned KTEvent::GetRecordAt(unsigned int iPoint) const
    {
        return fRecord[iPoint];
    }

    inline unsigned KTEvent::GetRecordAtTime(double time) const
    {
        return this->GetRecordAt((unsigned int)(nint(std::max(0., time) / fBinWidth)));
    }

    inline void KTEvent::SetFrameID(const std::vector< unsigned >& frameID)
    {
        this->fFrameID = frameID;
    }

    inline void KTEvent::SetRecord(const std::vector< unsigned >& record)
    {
        this->fRecord = record;
    }

    inline void KTEvent::SetRecordLength(double recordLength)
    {
        this->fRecordLength = recordLength;
    }

    inline void KTEvent::SetSampleRate(double sampleRate)
    {
        this->fSampleRate = sampleRate;
    }

    inline void KTEvent::SetBinWidth(double binWidth)
    {
        this->fBinWidth = binWidth;
    }

    inline void KTEvent::SetTimeStamp(const std::vector< unsigned >& timeStamp)
    {
        this->fTimeStamp = timeStamp;
    }


    inline int KTEvent::nint(double x) const
    {
        int i;
        if (x >= 0.)
        {
            i = int(x + 0.5);
            if (x + 0.5 == double(i) && (i & 1)) i--;
        }
        else
        {
            i = int(x - 0.5);
            if (x - 0.5 == double(i) && (i & 1)) i++;
        }
        return i;
    }


} /* namespace Katydid */

#endif /* KTEVENT_HH_ */
