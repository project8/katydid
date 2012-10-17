/**
 @file KTTimeSeriesDataFFTW.hh
 @brief Contains KTTimeSeriesDataFFTW
 @details Version of KTTimeSeriesData containing a KTTimeSeriesFFTW.
 @author: N. S. Oblath
 @date: Oct 16, 2012
 */

#ifndef KTTIMESERIESDATAFFTW_HH_
#define KTTIMESERIESDATAFFTW_HH_

#include "KTTimeSeriesData.hh"


namespace Katydid
{
    class KTBasicTimeSeriesDataFFTW : public KTBasicTimeSeriesData
    {
        public:
        KTBasicTimeSeriesDataFFTW(UInt_t nChannels=1);
            virtual ~KTBasicTimeSeriesDataFFTW();

            const std::string& GetName() const;
            static const std::string& StaticGetName();

            void Accept(KTWriter* writer) const;

        private:
            static std::string fName;


        public:
            UInt_t GetNChannels() const;

            const KTTimeSeries* GetRecord(UInt_t channelNum = 0) const;
            KTTimeSeries* GetRecord(UInt_t channelNum = 0);

            void SetNChannels(UInt_t channels);

            void SetRecord(KTTimeSeries* record, UInt_t channelNum = 0);

        protected:
            std::vector< KTTimeSeries* > fChannelData;

    };

    inline const std::string& KTBasicTimeSeriesDataFFTW::GetName() const
    {
        return fName;
    }

    inline UInt_t KTBasicTimeSeriesDataFFTW::GetNChannels() const
    {
        return UInt_t(fChannelData.size());
    }

    inline KTTimeSeries* KTBasicTimeSeriesDataFFTW::GetRecord(UInt_t channelNum)
    {
        return fChannelData[channelNum];
    }

    inline const KTTimeSeries* KTBasicTimeSeriesDataFFTW::GetRecord(UInt_t channelNum) const
    {
        return fChannelData[channelNum];
    }

    inline void KTBasicTimeSeriesDataFFTW::SetNChannels(UInt_t channels)
    {
        fChannelData.resize(channels);
        return;
    }

    inline void KTBasicTimeSeriesDataFFTW::SetRecord(KTTimeSeries* record, UInt_t channelNum)
    {
        if (channelNum >= fChannelData.size()) fChannelData.resize(channelNum+1);
        fChannelData[channelNum] = record;
        return;
    }




    class KTTimeSeriesDataFFTW : public KTTimeSeriesData
    {
        public:
            KTTimeSeriesDataFFTW(UInt_t nChannels=1);
            virtual ~KTTimeSeriesDataFFTW();

            const std::string& GetName() const;
            static const std::string& StaticGetName();

            void Accept(KTWriter* writer) const;

        private:
            static std::string fName;

    };

    inline const std::string& KTTimeSeriesDataFFTW::GetName() const
    {
        return fName;
    }

} /* namespace Katydid */

#endif /* KTTIMESERIESDATAFFTW_HH_ */
