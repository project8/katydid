/**
 @file KTTimeSeriesData.hh
 @brief Contains KTTimeSeriesData
 @details Contains the information from a single Egg event in the form of a 1-D std::vector of unsigned integers.
 The data are the time series of the event.
 @note Prior to August 24, 2012, this class was called KTEvent.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTTIMESERIESDATA_HH_
#define KTTIMESERIESDATA_HH_

#include "KTData.hh"

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
                std::vector< DataType >* fRecord;
            };

        public:
            KTTimeSeriesData(unsigned nChannels=1);
            virtual ~KTTimeSeriesData();

            const std::string& GetName() const;
            static const std::string& StaticGetName();

#ifdef ROOT_FOUND
            virtual TH1C* CreateTimeSeriesHistogram(unsigned channelNum = 0) const;
            virtual TH1I* CreateAmplitudeDistributionHistogram(unsigned channelNum = 0) const;
#endif

            unsigned GetNChannels() const;

            unsigned GetRecordSize() const;
            double GetSampleRate() const;
            double GetRecordLength() const;
            double GetBinWidth() const;

            ClockType GetTimeStamp(unsigned channelNum = 0) const;
            ChIdType GetChannelID(unsigned channelNum = 0) const;
            AcqIdType GetAcquisitionID(unsigned channelNum = 0) const;
            RecIdType GetRecordID(unsigned channelNum = 0) const;

            const std::vector< DataType >* GetRecord(unsigned channelNum = 0) const;
            std::vector< DataType >* GetRecord(unsigned channelNum = 0);

            //unsigned GetRecordSize(unsigned channelNum = 0) const;
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

        private:
            unsigned fRecordSize; // number of bins
            double fSampleRate; // in Hz
            double fRecordLength; // in sec
            double fBinWidth; // in sec

            std::vector< PerChannelData > fChannelData;

            static std::string fName;

        private:
            /// Round to nearest integer. Rounds half integers to the nearest even integer.
            /// Based on ROOT's TMath::Nint(Double_t)
            int nint(double x) const;

    };

    inline const std::string& KTTimeSeriesData::GetName() const
    {
        return fName;
    }

    inline unsigned KTTimeSeriesData::GetNChannels() const
    {
        return unsigned(fChannelData.size());
    }

    inline DataType KTTimeSeriesData::GetRecordAt(unsigned int iPoint, unsigned channelNum) const
    {
        return fChannelData[channelNum].fRecord->at(iPoint);
    }

    inline DataType KTTimeSeriesData::GetRecordAtTime(double time, unsigned channelNum) const
    {
        return this->GetRecordAt((unsigned)(nint(std::max(0., time) / fBinWidth)), channelNum);
    }

    template< typename XType >
    XType KTTimeSeriesData::GetRecordAt(unsigned iPoint, unsigned channelNum) const
    {
        return (XType)GetRecordAt(iPoint, channelNum);
    }

    template< typename XType >
    XType KTTimeSeriesData::GetRecordAtTime(double time, unsigned channelNum) const
    {
        return this->GetRecordAt< XType >((unsigned)(nint(std::max(0., time) / fBinWidth)), channelNum);
    }

    inline unsigned KTTimeSeriesData::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline double KTTimeSeriesData::GetRecordLength() const
    {
        return fRecordLength;
    }

    inline double KTTimeSeriesData::GetSampleRate() const
    {
        return fSampleRate;
    }

    inline double KTTimeSeriesData::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline ClockType KTTimeSeriesData::GetTimeStamp(unsigned channelNum) const
    {
        return fChannelData[channelNum].fTimeStamp;
    }

    inline ChIdType KTTimeSeriesData::GetChannelID(unsigned channelNum) const
    {
        return fChannelData[channelNum].fChannelID;
    }

    inline AcqIdType KTTimeSeriesData::GetAcquisitionID(unsigned channelNum) const
    {
        return fChannelData[channelNum].fAcquisitionID;
    }

    inline RecIdType KTTimeSeriesData::GetRecordID(unsigned channelNum) const
    {
        return fChannelData[channelNum].fRecordID;
    }

    inline const std::vector< DataType >* KTTimeSeriesData::GetRecord(unsigned channelNum) const
    {
        return fChannelData[channelNum].fRecord;
    }

    inline std::vector< DataType >* KTTimeSeriesData::GetRecord(unsigned channelNum)
    {
        return fChannelData[channelNum].fRecord;
    }

    inline void KTTimeSeriesData::SetRecordSize(unsigned recordSize)
    {
        this->fRecordSize = recordSize;
    }

    inline void KTTimeSeriesData::SetRecordLength(double recordLength)
    {
        this->fRecordLength = recordLength;
    }

    inline void KTTimeSeriesData::SetSampleRate(double sampleRate)
    {
        this->fSampleRate = sampleRate;
    }

    inline void KTTimeSeriesData::SetBinWidth(double binWidth)
    {
        this->fBinWidth = binWidth;
    }

    inline void KTTimeSeriesData::CalculateBinWidthAndRecordLength()
    {
        SetBinWidth(1. / fSampleRate);
        SetRecordLength(double(fRecordSize) * fBinWidth);
        return;
    }

    inline void KTTimeSeriesData::SetNChannels(unsigned channels)
    {
        fChannelData.resize(channels);
        return;
    }

    inline void KTTimeSeriesData::SetTimeStamp(ClockType timeStamp, unsigned channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fTimeStamp = timeStamp;
        return;
    }

    inline void KTTimeSeriesData::SetChannelID(ChIdType chId, unsigned channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fChannelID = chId;
        return;
    }

    inline void KTTimeSeriesData::SetAcquisitionID(AcqIdType acqId, unsigned channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fAcquisitionID = acqId;
        return;
    }

    inline void KTTimeSeriesData::SetRecordID(RecIdType recId, unsigned channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fRecordID = recId;
        return;
    }

    inline void KTTimeSeriesData::SetRecord(std::vector< DataType >* record, unsigned channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fRecord = record;
    }



    inline int KTTimeSeriesData::nint(double x) const
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

#endif /* KTTIMESERIESDATA_HH_ */
