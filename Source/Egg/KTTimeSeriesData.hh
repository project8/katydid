/**
 @file KTTimeSeriesData.hh
 @brief Contains KTTimeSeriesData
 @details Contains the information from a single Egg event in the form of a 1-D std::vector of UInt_tegers.
 The data are the time series of the event.
 @note Prior to August 24, 2012, this class was called KTEvent.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTTIMESERIESDATA_HH_
#define KTTIMESERIESDATA_HH_

#include "KTData.hh"

#include "KTMath.hh"
#include "KTTimeSeries.hh"

#include "MonarchTypes.hpp"

#include <algorithm>
#include <vector>

#ifdef ROOT_FOUND
class TH1C;
class TH1I;
#endif

namespace Katydid
{
    class KTTimeSeriesData : public KTData
    {
        protected:
            struct PerChannelData
            {
                ClockType fTimeStamp;
                ChIdType fChannelID;
                AcqIdType fAcquisitionID;
                RecIdType fRecordID;
                KTTimeSeries* fRecord;
            };

        public:
            KTTimeSeriesData(UInt_t nChannels=1);
            virtual ~KTTimeSeriesData();

            const std::string& GetName() const;
            static const std::string& StaticGetName();

#ifdef ROOT_FOUND
            virtual TH1C* CreateTimeSeriesHistogram(UInt_t channelNum = 0) const;
            virtual TH1I* CreateAmplitudeDistributionHistogram(UInt_t channelNum = 0) const;
#endif

            UInt_t GetNChannels() const;

            UInt_t GetRecordSize() const;
            Double_t GetSampleRate() const;
            Double_t GetRecordLength() const;
            Double_t GetBinWidth() const;

            ClockType GetTimeStamp(UInt_t channelNum = 0) const;
            ChIdType GetChannelID(UInt_t channelNum = 0) const;
            AcqIdType GetAcquisitionID(UInt_t channelNum = 0) const;
            RecIdType GetRecordID(UInt_t channelNum = 0) const;

            const KTTimeSeries* GetRecord(UInt_t channelNum = 0) const;
            KTTimeSeries* GetRecord(UInt_t channelNum = 0);

            //UInt_t GetRecordSize(UInt_t channelNum = 0) const;
            Double_t GetRecordAt(UInt_t iBin, UInt_t channelNum = 0) const;
            template< typename XType >
            XType GetRecordAt(UInt_t iBin, UInt_t channelNum = 0) const;
            DataType GetRecordAtTime(Double_t time, UInt_t channelNum = 0) const; /// time is in seconds and >= 0
            template< typename XType >
            XType GetRecordAtTime(Double_t time, UInt_t channelNum = 0) const; /// time is in seconds and >= 0

            void SetNChannels(UInt_t channels);

            void SetRecordSize(UInt_t size);
            void SetSampleRate(Double_t sampleRate);
            void SetRecordLength(Double_t recordLength);
            void SetBinWidth(Double_t binWidth);
            void CalculateBinWidthAndRecordLength();

            void SetTimeStamp(ClockType timeStamp, UInt_t channelNum = 0);
            void SetChannelID(ChIdType chId, UInt_t channelNum = 0);
            void SetAcquisitionID(AcqIdType acqId, UInt_t channelNum = 0);
            void SetRecordID(RecIdType recId, UInt_t channelNum = 0);

            void SetRecord(KTTimeSeries* record, UInt_t channelNum = 0);

        private:
            UInt_t fRecordSize; // number of bins
            Double_t fSampleRate; // in Hz
            Double_t fRecordLength; // in sec
            Double_t fBinWidth; // in sec

            std::vector< PerChannelData > fChannelData;

            static std::string fName;

    };

    inline const std::string& KTTimeSeriesData::GetName() const
    {
        return fName;
    }

    inline UInt_t KTTimeSeriesData::GetNChannels() const
    {
        return UInt_t(fChannelData.size());
    }

    inline Double_t KTTimeSeriesData::GetRecordAt(UInt_t iPoint, UInt_t channelNum) const
    {
        return (*(fChannelData[channelNum].fRecord))(iPoint);
    }

    inline DataType KTTimeSeriesData::GetRecordAtTime(Double_t time, UInt_t channelNum) const
    {
        return this->GetRecordAt((UInt_t)(KTMath::Nint(std::max(0., time) / fBinWidth)), channelNum);
    }

    template< typename XType >
    XType KTTimeSeriesData::GetRecordAt(UInt_t iPoint, UInt_t channelNum) const
    {
        return (XType)GetRecordAt(iPoint, channelNum);
    }

    template< typename XType >
    XType KTTimeSeriesData::GetRecordAtTime(Double_t time, UInt_t channelNum) const
    {
        return this->GetRecordAt< XType >((UInt_t)(KTMath::Nint(std::max(0., time) / fBinWidth)), channelNum);
    }

    inline UInt_t KTTimeSeriesData::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline Double_t KTTimeSeriesData::GetRecordLength() const
    {
        return fRecordLength;
    }

    inline Double_t KTTimeSeriesData::GetSampleRate() const
    {
        return fSampleRate;
    }

    inline Double_t KTTimeSeriesData::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline ClockType KTTimeSeriesData::GetTimeStamp(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fTimeStamp;
    }

    inline ChIdType KTTimeSeriesData::GetChannelID(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fChannelID;
    }

    inline AcqIdType KTTimeSeriesData::GetAcquisitionID(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fAcquisitionID;
    }

    inline RecIdType KTTimeSeriesData::GetRecordID(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fRecordID;
    }

    inline const KTTimeSeries* KTTimeSeriesData::GetRecord(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fRecord;
    }

    inline KTTimeSeries* KTTimeSeriesData::GetRecord(UInt_t channelNum)
    {
        return fChannelData[channelNum].fRecord;
    }

    inline void KTTimeSeriesData::SetRecordSize(UInt_t recordSize)
    {
        this->fRecordSize = recordSize;
    }

    inline void KTTimeSeriesData::SetRecordLength(Double_t recordLength)
    {
        this->fRecordLength = recordLength;
    }

    inline void KTTimeSeriesData::SetSampleRate(Double_t sampleRate)
    {
        this->fSampleRate = sampleRate;
    }

    inline void KTTimeSeriesData::SetBinWidth(Double_t binWidth)
    {
        this->fBinWidth = binWidth;
    }

    inline void KTTimeSeriesData::CalculateBinWidthAndRecordLength()
    {
        SetBinWidth(1. / fSampleRate);
        SetRecordLength(Double_t(fRecordSize) * fBinWidth);
        return;
    }

    inline void KTTimeSeriesData::SetNChannels(UInt_t channels)
    {
        fChannelData.resize(channels);
        return;
    }

    inline void KTTimeSeriesData::SetTimeStamp(ClockType timeStamp, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fTimeStamp = timeStamp;
        return;
    }

    inline void KTTimeSeriesData::SetChannelID(ChIdType chId, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fChannelID = chId;
        return;
    }

    inline void KTTimeSeriesData::SetAcquisitionID(AcqIdType acqId, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fAcquisitionID = acqId;
        return;
    }

    inline void KTTimeSeriesData::SetRecordID(RecIdType recId, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fRecordID = recId;
        return;
    }

    inline void KTTimeSeriesData::SetRecord(KTTimeSeries* record, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fRecord = record;
    }

} /* namespace Katydid */

#endif /* KTTIMESERIESDATA_HH_ */
