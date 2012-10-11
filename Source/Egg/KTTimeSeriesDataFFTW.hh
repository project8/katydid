/**
 @file KTTimeSeriesDataFFTW.hh
 @brief Contains KTTimeSeriesDataFFTW
 @details Contains the information from a single Egg event in the form of a 1-D std::vector of UInt_tegers.
 The data are the time series of the event.
 @note Prior to August 24, 2012, this class was called KTEvent.
 @author: N. S. Oblath
 @date: Sep 9, 2011
 */

#ifndef KTTIMESERIESDATAFFTW_HH_
#define KTTIMESERIESDATAFFTW_HH_

#include "KTData.hh"

#include "KTMath.hh"
#include "KTTimeSeriesFFTW.hh"

#include "MonarchTypes.hpp"

#include <algorithm>
#include <vector>

#ifdef ROOT_FOUND
class TH1C;
class TH1I;
#endif

namespace Katydid
{
    class KTTimeSeriesDataFFTW : public KTData
    {
        protected:
            struct PerChannelData
            {
                ClockType fTimeStamp;
                ChIdType fChannelID;
                AcqIdType fAcquisitionID;
                RecIdType fRecordID;
                KTTimeSeriesFFTW* fRecord;
            };

        public:
            KTTimeSeriesDataFFTW(UInt_t nChannels=1);
            virtual ~KTTimeSeriesDataFFTW();

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

            const KTTimeSeriesFFTW* GetRecord(UInt_t channelNum = 0) const;
            KTTimeSeriesFFTW* GetRecord(UInt_t channelNum = 0);

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

            void SetRecord(KTTimeSeriesFFTW* record, UInt_t channelNum = 0);

        private:
            UInt_t fRecordSize; // number of bins
            Double_t fSampleRate; // in Hz
            Double_t fRecordLength; // in sec
            Double_t fBinWidth; // in sec

            std::vector< PerChannelData > fChannelData;

            static std::string fName;

    };

    inline const std::string& KTTimeSeriesDataFFTW::GetName() const
    {
        return fName;
    }

    inline UInt_t KTTimeSeriesDataFFTW::GetNChannels() const
    {
        return UInt_t(fChannelData.size());
    }

    inline Double_t KTTimeSeriesDataFFTW::GetRecordAt(UInt_t iPoint, UInt_t channelNum) const
    {
        return (*(fChannelData[channelNum].fRecord))(iPoint)[0];
    }

    inline DataType KTTimeSeriesDataFFTW::GetRecordAtTime(Double_t time, UInt_t channelNum) const
    {
        return this->GetRecordAt((UInt_t)(KTMath::Nint(std::max(0., time) / fBinWidth)), channelNum);
    }

    template< typename XType >
    XType KTTimeSeriesDataFFTW::GetRecordAt(UInt_t iPoint, UInt_t channelNum) const
    {
        return (XType)GetRecordAt(iPoint, channelNum);
    }

    template< typename XType >
    XType KTTimeSeriesDataFFTW::GetRecordAtTime(Double_t time, UInt_t channelNum) const
    {
        return this->GetRecordAt< XType >((UInt_t)(KTMath::Nint(std::max(0., time) / fBinWidth)), channelNum);
    }

    inline UInt_t KTTimeSeriesDataFFTW::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline Double_t KTTimeSeriesDataFFTW::GetRecordLength() const
    {
        return fRecordLength;
    }

    inline Double_t KTTimeSeriesDataFFTW::GetSampleRate() const
    {
        return fSampleRate;
    }

    inline Double_t KTTimeSeriesDataFFTW::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline ClockType KTTimeSeriesDataFFTW::GetTimeStamp(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fTimeStamp;
    }

    inline ChIdType KTTimeSeriesDataFFTW::GetChannelID(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fChannelID;
    }

    inline AcqIdType KTTimeSeriesDataFFTW::GetAcquisitionID(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fAcquisitionID;
    }

    inline RecIdType KTTimeSeriesDataFFTW::GetRecordID(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fRecordID;
    }

    inline const KTTimeSeriesFFTW* KTTimeSeriesDataFFTW::GetRecord(UInt_t channelNum) const
    {
        return fChannelData[channelNum].fRecord;
    }

    inline KTTimeSeriesFFTW* KTTimeSeriesDataFFTW::GetRecord(UInt_t channelNum)
    {
        return fChannelData[channelNum].fRecord;
    }

    inline void KTTimeSeriesDataFFTW::SetRecordSize(UInt_t recordSize)
    {
        this->fRecordSize = recordSize;
    }

    inline void KTTimeSeriesDataFFTW::SetRecordLength(Double_t recordLength)
    {
        this->fRecordLength = recordLength;
    }

    inline void KTTimeSeriesDataFFTW::SetSampleRate(Double_t sampleRate)
    {
        this->fSampleRate = sampleRate;
    }

    inline void KTTimeSeriesDataFFTW::SetBinWidth(Double_t binWidth)
    {
        this->fBinWidth = binWidth;
    }

    inline void KTTimeSeriesDataFFTW::CalculateBinWidthAndRecordLength()
    {
        SetBinWidth(1. / fSampleRate);
        SetRecordLength(Double_t(fRecordSize) * fBinWidth);
        return;
    }

    inline void KTTimeSeriesDataFFTW::SetNChannels(UInt_t channels)
    {
        fChannelData.resize(channels);
        return;
    }

    inline void KTTimeSeriesDataFFTW::SetTimeStamp(ClockType timeStamp, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fTimeStamp = timeStamp;
        return;
    }

    inline void KTTimeSeriesDataFFTW::SetChannelID(ChIdType chId, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fChannelID = chId;
        return;
    }

    inline void KTTimeSeriesDataFFTW::SetAcquisitionID(AcqIdType acqId, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fAcquisitionID = acqId;
        return;
    }

    inline void KTTimeSeriesDataFFTW::SetRecordID(RecIdType recId, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fRecordID = recId;
        return;
    }

    inline void KTTimeSeriesDataFFTW::SetRecord(KTTimeSeriesFFTW* record, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum].fRecord = record;
    }

} /* namespace Katydid */

#endif /* KTTIMESERIESDATAFFTW_HH_ */
