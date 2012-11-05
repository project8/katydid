/*
 * KTPairedTimeSeriesData.hh
 *
 *  Created on: Aug 27, 2012
 *      Author: nsoblath
 */

#ifndef KTPAIREDTIMESERIESDATA_HH_
#define KTPAIREDTIMESERIESDATA_HH_

#include "KTWriteableData.hh"

#include <vector>

namespace Katydid
{
    class KTTimeSeries;

    class KTPairedTimeSeriesData : public KTWriteableData
    {
        protected:
            struct PerPairData
            {
                UInt_t fFirstChannel;
                UInt_t fSecondChannel;
                KTTimeSeries* fCorrelation;
            };

        public:
            KTPairedTimeSeriesData(unsigned nChannels=1);
            virtual ~KTPairedTimeSeriesData();

            const std::string& GetName() const;
            static const std::string& StaticGetName();

            const KTTimeSeries* GetCorrelation(UInt_t pairNum = 0) const;
            KTTimeSeries* GetCorrelation(UInt_t pairNum = 0);
            UInt_t GetFirstChannel(UInt_t pairNum = 0) const;
            UInt_t GetSecondChannel(UInt_t pairNum = 0) const;
            UInt_t GetNPairs() const;

            void SetCorrelation(KTTimeSeries* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t pairNum = 0);
            void SetNPairs(unsigned pairs);

            void Accept(KTWriter* writer) const;

        protected:
            static std::string fName;

            std::vector< PerPairData > fData;

    };

    inline const std::string& KTPairedTimeSeriesData::GetName() const
    {
        return fName;
    }

    inline const KTTimeSeries* KTPairedTimeSeriesData::GetCorrelation(UInt_t pairNum) const
    {
        return fData[pairNum].fCorrelation;
    }

    inline KTTimeSeries* KTPairedTimeSeriesData::GetCorrelation(UInt_t pairNum)
    {
        return fData[pairNum].fCorrelation;
    }

    inline UInt_t KTPairedTimeSeriesData::GetFirstChannel(UInt_t pairNum) const
    {
        return fData[pairNum].fFirstChannel;
    }

    inline UInt_t KTPairedTimeSeriesData::GetSecondChannel(UInt_t pairNum) const
    {
        return fData[pairNum].fSecondChannel;
    }

    inline UInt_t KTPairedTimeSeriesData::GetNPairs() const
    {
        return UInt_t(fData.size());
    }

    inline void KTPairedTimeSeriesData::SetCorrelation(KTTimeSeries* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t pairNum)
    {
        if (pairNum >= fData.size()) fData.resize(pairNum+1);
        fData[pairNum].fCorrelation = record;
        fData[pairNum].fFirstChannel = firstChannel;
        fData[pairNum].fSecondChannel = secondChannel;
    }

    inline void KTPairedTimeSeriesData::SetNPairs(unsigned pairs)
    {
        fData.resize(pairs);
        return;
    }

} /* namespace Katydid */

#endif /* KTPAIREDTIMESERIESDATA_HH_ */
