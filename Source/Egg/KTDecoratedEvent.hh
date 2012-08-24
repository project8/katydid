/**
 @file KTDecoratedEvent.hh
 @brief Contains KTDecoratedEvent
 @details Base decorated-event class, implementing the Decorator pattern around KTEvent.
 This class provides the standard KTEvent interface for any "decorated" event.
 @author: N. S. Oblath
 @date: August 24, 2012
 */

#ifndef KTDECORATEDEVENT_HH_
#define KTDECORATEDEVENT_HH_

#include "KTAbstractEvent.hh"


namespace Katydid
{
    class KTDecoratedEvent : public KTAbstractEvent
    {
        public:
            KTDecoratedEvent(KTAbstractEvent* event);
            virtual ~KTDecoratedEvent();

        private:
            KTDecoratedEvent();

        protected:
            KTAbstractEvent* fEvent;

            //*****************************
            // Standard Event Interface
            //*****************************
        public:

#ifdef ROOT_FOUND
            virtual TH1C* CreateEventHistogram(unsigned channelNum = 0) const;
            virtual TH1I* CreateAmplitudeDistributionHistogram(unsigned channelNum = 0) const;
#endif

            unsigned GetNRecords() const;

            unsigned GetRecordSize() const;
            double GetSampleRate() const;
            double GetRecordLength() const;
            double GetBinWidth() const;

            ClockType GetTimeStamp(unsigned channelNum = 0) const;
            ChIdType GetChannelID(unsigned channelNum = 0) const;
            AcqIdType GetAcquisitionID(unsigned channelNum = 0) const;
            RecIdType GetRecordID(unsigned channelNum = 0) const;

            const std::vector< DataType >& GetRecord(unsigned channelNum = 0) const;

            DataType GetRecordAt(unsigned int iBin, unsigned channelNum = 0) const;
            template< typename XType >
            XType GetRecordAt(unsigned int iBin, unsigned channelNum = 0) const;
            DataType GetRecordAtTime(double time, unsigned channelNum = 0) const; /// time is in seconds and >= 0
            template< typename XType >
            XType GetRecordAtTime(double time, unsigned channelNum = 0) const; /// time is in seconds and >= 0

            void SetNChannels(unsigned channels);

            void SetRecordSize(unsigned size);
            void SetSampleRate(double sampleRate);
            void SetRecordLength(double recordLength);
            void SetBinWidth(double binWidth);
            void CalculateBinWidthAndRecordLength();

            void SetTimeStamp(ClockType timeStamp, unsigned channelNum = 0);
            void SetChannelID(ChIdType chId, unsigned channelNum = 0);
            void SetAcquisitionID(AcqIdType acqId, unsigned channelNum = 0);
            void SetRecordID(RecIdType recId, unsigned channelNum = 0);

            void SetRecord(std::vector< DataType >* record, unsigned channelNum = 0);

    };

    template< typename XType >
    XType KTDecoratedEvent::GetRecordAt(unsigned iPoint, unsigned channelNum) const
    {
        return fEvent->GetRecordAt< XType >(iPoint, channelNum);
    }

    template< typename XType >
    XType KTDecoratedEvent::GetRecordAtTime(double time, unsigned channelNum) const
    {
        return fEvent->GetRecordAt< XType >(time, channelNum);
    }

    inline unsigned KTDecoratedEvent::GetNRecords() const
    {
        return fEvent->GetNRecords();
    }

    inline DataType KTDecoratedEvent::GetRecordAt(unsigned int iPoint, unsigned channelNum) const
    {
        return fEvent->GetRecordAt(iPoint, channelNum);
    }

    inline DataType KTDecoratedEvent::GetRecordAtTime(double time, unsigned channelNum) const
    {
        return fEvent->GetRecordAtTime(time, channelNum);
    }

    inline unsigned KTDecoratedEvent::GetRecordSize() const
    {
        return fEvent->GetRecordSize();
    }

    inline double KTDecoratedEvent::GetRecordLength() const
    {
        return fEvent->GetRecordLength();
    }

    inline double KTDecoratedEvent::GetSampleRate() const
    {
        return fEvent->GetSampleRate();
    }

    inline double KTDecoratedEvent::GetBinWidth() const
    {
        return fEvent->GetBinWidth();
    }

    inline ClockType KTDecoratedEvent::GetTimeStamp(unsigned channelNum) const
    {
        return fEvent->GetTimeStamp(channelNum);
    }

    inline ChIdType KTDecoratedEvent::GetChannelID(unsigned channelNum) const
    {
        return fEvent->GetChannelID(channelNum);
    }

    inline AcqIdType KTDecoratedEvent::GetAcquisitionID(unsigned channelNum) const
    {
        return fEvent->GetAcquisitionID(channelNum);
    }

    inline RecIdType KTDecoratedEvent::GetRecordID(unsigned channelNum) const
    {
        return fEvent->GetRecordID(channelNum);
    }

    inline const std::vector< DataType >& KTDecoratedEvent::GetRecord(unsigned channelNum) const
    {
        return fEvent->GetRecord(channelNum);
    }

    inline void KTDecoratedEvent::SetRecordSize(unsigned recordSize)
    {
        fEvent->SetRecordSize(recordSize);
        return;
    }

    inline void KTDecoratedEvent::SetRecordLength(double recordLength)
    {
        fEvent->SetRecordLength(recordLength);
        return;
    }

    inline void KTDecoratedEvent::SetSampleRate(double sampleRate)
    {
        fEvent->SetSampleRate(sampleRate);
        return;
    }

    inline void KTDecoratedEvent::SetBinWidth(double binWidth)
    {
        fEvent->SetBinWidth(binWidth);
        return;
    }

    inline void KTDecoratedEvent::CalculateBinWidthAndRecordLength()
    {
        fEvent->CalculateBinWidthAndRecordLength();
        return;
    }

    inline void KTDecoratedEvent::SetNChannels(unsigned channels)
    {
        fEvent->SetNChannels(channels);
        return;
    }

    inline void KTDecoratedEvent::SetTimeStamp(ClockType timeStamp, unsigned channelNum)
    {
        fEvent->SetTimeStamp(timeStamp, channelNum);
        return;
    }

    inline void KTDecoratedEvent::SetChannelID(ChIdType chId, unsigned channelNum)
    {
        fEvent->SetChannelID(chId, channelNum);
        return;
    }

    inline void KTDecoratedEvent::SetAcquisitionID(AcqIdType acqId, unsigned channelNum)
    {
        fEvent->SetAcquisitionID(acqId, channelNum);
        return;
    }

    inline void KTDecoratedEvent::SetRecordID(RecIdType recId, unsigned channelNum)
    {
        fEvent->SetRecordID(recId, channelNum);
        return;
    }

    inline void KTDecoratedEvent::SetRecord(std::vector< DataType >* record, unsigned channelNum)
    {
        fEvent->SetRecord(record, channelNum);
        return;
    }

} /* namespace Katydid */

#endif /* KTDECORATEDEVENT_HH_ */
