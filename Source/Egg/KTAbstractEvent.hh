/**
 @file KTAbstractEvent.hh
 @brief Contains KTAbstractEvent
 @details Abstract interface for a KTEvent.
 @author: N. S. Oblath
 @date: August 24, 2012
 */

#ifndef KTABSTRACTEVENT_HH_
#define KTABSTRACTEVENT_HH_

#include "MonarchTypes.hpp"

#include <algorithm>
#include <vector>

#ifdef ROOT_FOUND
class TH1C;
class TH1I;
#endif

namespace Katydid
{
    class KTAbstractEvent
    {
        protected:
            struct PerChannelData
            {
                ClockType fTimeStamp;
                ChIdType fChannelID;
                AcqIdType fAcquisitionID;
                RecIdType fRecordID;
                std::vector< DataType >* fRecord;
            };

        public:
            KTAbstractEvent();
            virtual ~KTAbstractEvent();

#ifdef ROOT_FOUND
            virtual TH1C* CreateEventHistogram(unsigned channelNum = 0) const = 0;
            virtual TH1I* CreateAmplitudeDistributionHistogram(unsigned channelNum = 0) const = 0;
#endif

            virtual unsigned GetNRecords() const = 0;

            virtual unsigned GetRecordSize() const = 0;
            virtual double GetSampleRate() const = 0;
            virtual double GetRecordLength() const = 0;
            virtual double GetBinWidth() const = 0;

            virtual ClockType GetTimeStamp(unsigned channelNum = 0) const = 0;
            virtual ChIdType GetChannelID(unsigned channelNum = 0) const = 0;
            virtual AcqIdType GetAcquisitionID(unsigned channelNum = 0) const = 0;
            virtual RecIdType GetRecordID(unsigned channelNum = 0) const = 0;

            virtual const std::vector< DataType >& GetRecord(unsigned channelNum = 0) const = 0;

            //unsigned GetRecordSize(unsigned channelNum = 0) const;
            virtual DataType GetRecordAt(unsigned int iBin, unsigned channelNum = 0) const = 0;
            template< typename XType >
            XType GetRecordAt(unsigned int iBin, unsigned channelNum = 0) const ;
            virtual DataType GetRecordAtTime(double time, unsigned channelNum = 0) const = 0; /// time is in seconds and >= 0
            template< typename XType >
            XType GetRecordAtTime(double time, unsigned channelNum = 0) const; /// time is in seconds and >= 0

            virtual void SetNChannels(unsigned channels) = 0;

            virtual void SetRecordSize(unsigned size) = 0;
            virtual void SetSampleRate(double sampleRate) = 0;
            virtual void SetRecordLength(double recordLength) = 0;
            virtual void SetBinWidth(double binWidth) = 0;
            virtual void CalculateBinWidthAndRecordLength() = 0;

            virtual void SetTimeStamp(ClockType timeStamp, unsigned channelNum = 0) = 0;
            virtual void SetChannelID(ChIdType chId, unsigned channelNum = 0) = 0;
            virtual void SetAcquisitionID(AcqIdType acqId, unsigned channelNum = 0) = 0;
            virtual void SetRecordID(RecIdType recId, unsigned channelNum = 0) = 0;

            virtual void SetRecord(std::vector< DataType >* record, unsigned channelNum = 0) = 0;

        protected:
            /// Round to nearest integer. Rounds half integers to the nearest even integer.
            /// Based on ROOT's TMath::Nint(Double_t)
            int nint(double x) const;

    };

    template< typename XType >
    XType KTAbstractEvent::GetRecordAt(unsigned iPoint, unsigned channelNum) const
    {
        return (XType)GetRecordAt(iPoint, channelNum);
    }

    template< typename XType >
    XType KTAbstractEvent::GetRecordAtTime(double time, unsigned channelNum) const
    {
        return this->GetRecordAt< XType >((unsigned)(nint(std::max(0., time) / GetBinWidth())), channelNum);
    }

    inline int KTAbstractEvent::nint(double x) const
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

#endif /* KTABSTRACTEVENT_HH_ */
