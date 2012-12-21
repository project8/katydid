/**
 @file KTTimeSeriesChannelData.hh
 @brief Contains KTTimeSeriesChannelData
 @details Contains the information from a single Egg event in the form of a 1-D std::vector of UInt_tegers.
 The data are the time series of the event.
 @note Prior to August 24, 2012, this class was called KTEvent.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTTIMESERIESCHANNELDATA_HH_
#define KTTIMESERIESCHANNELDATA_HH_

#include "KTTimeSeriesData.hh"

#include "KTTimeSeries.hh"

#include "MonarchTypes.hpp"

#include <vector>

namespace Katydid
{
    class KTTimeSeriesChannelData : public KTTimeSeriesData
    {
        public:
            KTTimeSeriesChannelData();
            virtual ~KTTimeSeriesChannelData();

            // relic functions from previous incarnations of this class
            virtual UInt_t GetRecordSize() const = 0;
            virtual Double_t GetBinWidth() const = 0;

    };


    class KTBasicTimeSeriesData : public KTTimeSeriesChannelData
    {
        public:
            KTBasicTimeSeriesData(UInt_t nChannels=1);
            virtual ~KTBasicTimeSeriesData();

            void Accept(KTWriter* writer) const;

        public:
            UInt_t GetNTimeSeries() const;

            UInt_t GetRecordSize() const;
            Double_t GetBinWidth() const;

            const KTTimeSeries* GetTimeSeries(UInt_t channelNum = 0) const;
            KTTimeSeries* GetTimeSeries(UInt_t channelNum = 0);

            void SetNTimeSeries(UInt_t channels);

            void SetTimeSeries(KTTimeSeries* record, UInt_t channelNum = 0);

        protected:
            std::vector< KTTimeSeries* > fChannelData;

    };

    inline UInt_t KTBasicTimeSeriesData::GetNTimeSeries() const
    {
        return UInt_t(fChannelData.size());
    }

    inline UInt_t KTBasicTimeSeriesData::GetRecordSize() const
    {
        if (fChannelData.size() > 0)
        {
            return fChannelData[0]->GetNTimeBins();
        }
        return 0;
    }

    inline Double_t KTBasicTimeSeriesData::GetBinWidth() const
    {
        if (fChannelData.size() > 0)
        {
            return fChannelData[0]->GetTimeBinWidth();
        }
        return 0.;
    }

    inline KTTimeSeries* KTBasicTimeSeriesData::GetTimeSeries(UInt_t channelNum)
    {
        return fChannelData[channelNum];
    }

    inline const KTTimeSeries* KTBasicTimeSeriesData::GetTimeSeries(UInt_t channelNum) const
    {
        return fChannelData[channelNum];
    }

    inline void KTBasicTimeSeriesData::SetNTimeSeries(UInt_t channels)
    {
        fChannelData.resize(channels);
        return;
    }

    inline void KTBasicTimeSeriesData::SetTimeSeries(KTTimeSeries* record, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum] = record;
        return;
    }



    class KTProgenitorTimeSeriesData : public KTTimeSeriesChannelData
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
            KTProgenitorTimeSeriesData(UInt_t nChannels=1);
            virtual ~KTProgenitorTimeSeriesData();

            void Accept(KTWriter* writer) const;

        private:
            static std::string fName;

        public:
            UInt_t GetNTimeSeries() const;

            UInt_t GetRecordSize() const;
            Double_t GetSampleRate() const;
            Double_t GetRecordLength() const;
            Double_t GetBinWidth() const;

            ClockType GetTimeStamp(UInt_t channelNum = 0) const;
            ChIdType GetChannelID(UInt_t channelNum = 0) const;
            AcqIdType GetAcquisitionID(UInt_t channelNum = 0) const;
            RecIdType GetRecordID(UInt_t channelNum = 0) const;

            const KTTimeSeries* GetTimeSeries(UInt_t channelNum = 0) const;
            KTTimeSeries* GetTimeSeries(UInt_t channelNum = 0);

            void SetNTimeSeries(UInt_t channels);

            void SetRecordSize(UInt_t size);
            void SetSampleRate(Double_t sampleRate);
            void SetRecordLength(Double_t recordLength);
            void SetBinWidth(Double_t binWidth);
            void CalculateBinWidthAndRecordLength();

            void SetTimeStamp(ClockType timeStamp, UInt_t channelNum = 0);
            void SetChannelID(ChIdType chId, UInt_t channelNum = 0);
            void SetAcquisitionID(AcqIdType acqId, UInt_t channelNum = 0);
            void SetRecordID(RecIdType recId, UInt_t channelNum = 0);

            void SetTimeSeries(KTTimeSeries* record, UInt_t channelNum = 0);

        private:
            UInt_t fRecordSize; // number of bins
            Double_t fSampleRate; // in Hz
            Double_t fRecordLength; // in sec
            Double_t fBinWidth; // in sec

            std::vector< PerChannelData > fChannelData;

    };

    inline UInt_t KTProgenitorTimeSeriesData::GetNTimeSeries() const
    {
        return UInt_t(fChannelData.size());
    }

    inline UInt_t KTProgenitorTimeSeriesData::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline Double_t KTProgenitorTimeSeriesData::GetRecordLength() const
    {
        return fRecordLength;
    }

    inline Double_t KTProgenitorTimeSeriesData::GetSampleRate() const
    {
        return fSampleRate;
    }

    inline Double_t KTProgenitorTimeSeriesData::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline ClockType KTProgenitorTimeSeriesData::GetTimeStamp(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fTimeStamp;
    }

    inline ChIdType KTProgenitorTimeSeriesData::GetChannelID(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fChannelID;
    }

    inline AcqIdType KTProgenitorTimeSeriesData::GetAcquisitionID(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fAcquisitionID;
    }

    inline RecIdType KTProgenitorTimeSeriesData::GetRecordID(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fRecordID;
    }

    inline const KTTimeSeries* KTProgenitorTimeSeriesData::GetTimeSeries(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fRecord;
    }

    inline KTTimeSeries* KTProgenitorTimeSeriesData::GetTimeSeries(UInt_t channelNum)
    {
        return fChannelData[channelNum].fRecord;
    }

    inline void KTProgenitorTimeSeriesData::SetRecordSize(UInt_t recordSize)
    {
        this->fRecordSize = recordSize;
    }

    inline void KTProgenitorTimeSeriesData::SetRecordLength(Double_t recordLength)
    {
        this->fRecordLength = recordLength;
    }

    inline void KTProgenitorTimeSeriesData::SetSampleRate(Double_t sampleRate)
    {
        this->fSampleRate = sampleRate;
    }

    inline void KTProgenitorTimeSeriesData::SetBinWidth(Double_t binWidth)
    {
        this->fBinWidth = binWidth;
    }

    inline void KTProgenitorTimeSeriesData::CalculateBinWidthAndRecordLength()
    {
        SetBinWidth(1. / fSampleRate);
        SetRecordLength(Double_t(fRecordSize) * fBinWidth);
        return;
    }

    inline void KTProgenitorTimeSeriesData::SetNTimeSeries(UInt_t channels)
    {
        fChannelData.resize(channels);
        return;
    }

    inline void KTProgenitorTimeSeriesData::SetTimeStamp(ClockType timeStamp, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fTimeStamp = timeStamp;
        return;
    }

    inline void KTProgenitorTimeSeriesData::SetChannelID(ChIdType chId, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fChannelID = chId;
        return;
    }

    inline void KTProgenitorTimeSeriesData::SetAcquisitionID(AcqIdType acqId, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fAcquisitionID = acqId;
        return;
    }

    inline void KTProgenitorTimeSeriesData::SetRecordID(RecIdType recId, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fRecordID = recId;
        return;
    }

    inline void KTProgenitorTimeSeriesData::SetTimeSeries(KTTimeSeries* record, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fRecord = record;
        return;
    }

} /* namespace Katydid */

#endif /* KTTIMESERIESCHANNELDATA_HH_ */
