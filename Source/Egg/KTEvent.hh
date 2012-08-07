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

#include "MonarchTypes.hpp"

#include <algorithm>
#include <vector>

#ifdef ROOT_FOUND
class TH1C;
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
            virtual TH1C* CreateEventHistogram(unsigned channelNum = 0) const;
            virtual TH1I* CreateAmplitudeDistributionHistogram(unsigned channelNum = 0) const;
#endif

            double GetSampleRate() const;
            double GetRecordLength() const;
            double GetBinWidth() const;

            ClockType GetTimeStamp(unsigned channelNum = 0) const;
            ChIdType GetChannelID(unsigned channelNum = 0) const;
            AcqIdType GetAcquisitionID(unsigned channelNum = 0) const;
            RecIdType GetRecordID(unsigned channelNum = 0) const;

            const std::vector< DataType >& GetRecord(unsigned channelNum = 0) const;

            unsigned GetRecordSize(unsigned channelNum = 0) const;
            DataType GetRecordAt(unsigned int iBin, unsigned channelNum = 0) const;
            template< typename XType >
            XType GetRecordAt(unsigned int iBin, unsigned channelNum = 0) const;
            DataType GetRecordAtTime(double time, unsigned channelNum = 0) const; /// time is in seconds and >= 0
            template< typename XType >
            XType GetRecordAtTime(double time, unsigned channelNum = 0) const; /// time is in seconds and >= 0

            void SetSampleRate(double sampleRate, unsigned channelNum = 0);
            void SetRecordLength(double recordLength, unsigned channelNum = 0);
            void SetBinWidth(double binWidth, unsigned channelNum = 0);

            void SetTimeStamp(ClockType timeStamp, unsigned channelNum = 0);
            void SetChannelID(ChIdType chId, unsigned channelNum = 0);
            void SetAcquisitionID(AcqIdType acqId, unsigned channelNum = 0);
            void SetRecordID(RecIdType recId, unsigned channelNum = 0);

            void SetRecord(const std::vector< DataType >& record, unsigned channelNum = 0);

        private:
            double fSampleRate; // in Hz
            double fRecordLength; // in sec
            double fBinWidth; // in sec
            std::vector< ClockType > fTimeStamps;
            std::vector< ChIdType > fChannelIDs;
            std::vector< AcqIdType > fAcquisitionIDs;
            std::vector< RecIdType > fRecordIDs;
            std::vector< std::vector< DataType > > fRecords;

        private:
            /// Round to nearest integer. Rounds half integers to the nearest even integer.
            /// Based on ROOT's TMath::Nint(Double_t)
            int nint(double x) const;

    };


    inline unsigned KTEvent::GetRecordSize(unsigned channelNum) const
    {
        return fRecords[channelNum].size();
    }

    inline DataType KTEvent::GetRecordAt(unsigned int iPoint, unsigned channelNum) const
    {
        return fRecords[iPoint];
    }

    inline DataType KTEvent::GetRecordAtTime(double time, unsigned channelNum) const
    {
        return this->GetRecordAt((unsigned)(nint(std::max(0., time) / fBinWidth)));
    }

    template< typename XType >
    XType KTEvent::GetRecordAt(unsigned iPoint, unsigned channelNum) const
    {
        return (XType)GetRecordAt(iPoint, channelNum);
    }

    template< typename XType >
    XType KTEvent::GetRecordAtTime(double time, unsigned channelNum) const
    {
        return this->GetRecordAt< XType >((unsigned)(nint(std::max(0., time) / fBinWidth)), channelNum);
    }


    inline double KTEvent::GetRecordLength() const
    {
        return fRecordLength;
    }

    inline double KTEvent::GetSampleRate() const
    {
        return fSampleRate;
    }

    inline double KTEvent::GetBinWidth() const
    {
        return fBinWidth;
    }

    ClockType KTEvent::GetTimeStamp(unsigned channelNum) const
    {
        return fTimeStamps[channelNum];
    }

    ChIdType KTEvent::GetChannelID(unsigned channelNum) const
    {
        return fChannelIDs[channelNum];
    }

    AcqIdType KTEvent::GetAcquisitionID(unsigned channelNum) const
    {
        return fAcquisitionIDs[channelNum];
    }

    RecIdType KTEvent::GetRecordID(unsigned channelNum) const
    {
        return fRecordIDs[channelNum];
    }

    inline const std::vector< DataType >& KTEvent::GetRecord(unsigned channelNum) const
    {
        return fRecords[channelNum];
    }


    inline void KTEvent::SetRecordLength(double recordLength, unsigned channelNum)
    {
        this->fRecordLength[channelNum] = recordLength;
    }

    inline void KTEvent::SetSampleRate(double sampleRate, unsigned channelNum)
    {
        this->fSampleRate[channelNum] = sampleRate;
    }

    inline void KTEvent::SetBinWidth(double binWidth, unsigned channelNum)
    {
        this->fBinWidth[channelNum] = binWidth;
    }

    void KTEvent::SetTimeStamp(ClockType timeStamp, unsigned channelNum)
    {
        fTimeStamps[channelNum] = timeStamp;
        return;
    }

    void KTEvent::SetChannelID(ChIdType chId, unsigned channelNum)
    {
        fChannelIDs[channelNum] = chId;
        return;
    }

    void KTEvent::SetAcquisitionID(AcqIdType acqId, unsigned channelNum)
    {
        fAcquisitionIDs[channelNum] = acqId;
        return;
    }

    void KTEvent::SetRecordID(RecIdType recId, unsigned channelNum)
    {
        fRecordIDs[channelNum] = recId;
        return;
    }

    inline void KTEvent::SetRecord(const std::vector< DataType >& record, unsigned channelNum)
    {
        this->fRecords[channelNum] = record;
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
