/*
 * KTTimeSeriesPairedData.hh
 *
 *  Created on: Aug 27, 2012
 *      Author: nsoblath
 */

#ifndef KTTIMESERIESPAIREDDATA_HH_
#define KTTIMESERIESPAIREDDATA_HH_

#include "KTTimeSeriesData.hh"

#include <vector>

namespace Katydid
{
    class KTTimeSeries;

    class KTTimeSeriesPairedData : public KTTimeSeriesData
    {
        protected:
            struct PerPairData
            {
                UInt_t fFirstChannel;
                UInt_t fSecondChannel;
                KTTimeSeries* fTimeSeries;
            };

        public:
            KTTimeSeriesPairedData(unsigned nChannels=1);
            virtual ~KTTimeSeriesPairedData();

            const KTTimeSeries* GetTimeSeries(UInt_t pairNum = 0) const;
            KTTimeSeries* GetTimeSeries(UInt_t pairNum = 0);
            UInt_t GetFirstChannel(UInt_t pairNum = 0) const;
            UInt_t GetSecondChannel(UInt_t pairNum = 0) const;
            UInt_t GetNTimeSeries() const;

            void SetTimeSeries(KTTimeSeries* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t pairNum = 0);
            void SetTimeSeries(KTTimeSeries* record, UInt_t pairNum = 0);

            void SetNTimeSeries(unsigned pairs);

            void Accept(KTWriter* writer) const;

        protected:
            static std::string fDefaultName;

            std::vector< PerPairData > fData;

    };

    inline const KTTimeSeries* KTTimeSeriesPairedData::GetTimeSeries(UInt_t pairNum) const
    {
        return fData[pairNum].fTimeSeries;
    }

    inline KTTimeSeries* KTTimeSeriesPairedData::GetTimeSeries(UInt_t pairNum)
    {
        return fData[pairNum].fTimeSeries;
    }

    inline UInt_t KTTimeSeriesPairedData::GetFirstChannel(UInt_t pairNum) const
    {
        return fData[pairNum].fFirstChannel;
    }

    inline UInt_t KTTimeSeriesPairedData::GetSecondChannel(UInt_t pairNum) const
    {
        return fData[pairNum].fSecondChannel;
    }

    inline UInt_t KTTimeSeriesPairedData::GetNTimeSeries() const
    {
        return UInt_t(fData.size());
    }

    inline void KTTimeSeriesPairedData::SetTimeSeries(KTTimeSeries* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t pairNum)
    {
        if (pairNum >= fData.size()) fData.resize(pairNum+1);
        fData[pairNum].fTimeSeries = record;
        fData[pairNum].fFirstChannel = firstChannel;
        fData[pairNum].fSecondChannel = secondChannel;
    }

    inline void KTTimeSeriesPairedData::SetTimeSeries(KTTimeSeries* record, UInt_t pairNum)
    {
        if (pairNum >= fData.size()) fData.resize(pairNum+1);
        fData[pairNum].fTimeSeries = record;
        fData[pairNum].fFirstChannel = 0;
        fData[pairNum].fSecondChannel = 0;
    }

    inline void KTTimeSeriesPairedData::SetNTimeSeries(unsigned pairs)
    {
        fData.resize(pairs);
        return;
    }

} /* namespace Katydid */

#endif /* KTTIMESERIESPAIREDDATA_HH_ */
