/*
 * KTCorrelationData.hh
 *
 *  Created on: Aug 27, 2012
 *      Author: nsoblath
 */

#ifndef KTCORRELATIONDATA_HH_
#define KTCORRELATIONDATA_HH_

#include "KTWriteableData.hh"

#include <vector>

namespace Katydid
{
    class KTFrequencySpectrum;

    class KTCorrelationData : public KTWriteableData
    {
        protected:
            struct PerPairData
            {
                UInt_t fFirstChannel;
                UInt_t fSecondChannel;
                KTFrequencySpectrum* fCorrelation;
            };

        public:
            KTCorrelationData(unsigned nChannels=1);
            virtual ~KTCorrelationData();

            const KTFrequencySpectrum* GetCorrelation(UInt_t pairNum = 0) const;
            KTFrequencySpectrum* GetCorrelation(UInt_t pairNum = 0);
            UInt_t GetFirstChannel(UInt_t pairNum = 0) const;
            UInt_t GetSecondChannel(UInt_t pairNum = 0) const;
            UInt_t GetNPairs() const;
            Double_t GetTimeInRun() const;

            void SetCorrelation(KTFrequencySpectrum* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t pairNum = 0);
            void SetNPairs(unsigned pairs);
            void SetTimeInRun(Double_t tir);

            void Accept(KTWriter* writer) const;

        protected:
            std::vector< PerPairData > fData;

            Double_t fTimeInRun;

    };

    inline const KTFrequencySpectrum* KTCorrelationData::GetCorrelation(UInt_t pairNum) const
    {
        return fData[pairNum].fCorrelation;
    }

    inline KTFrequencySpectrum* KTCorrelationData::GetCorrelation(UInt_t pairNum)
    {
        return fData[pairNum].fCorrelation;
    }

    inline UInt_t KTCorrelationData::GetFirstChannel(UInt_t pairNum) const
    {
        return fData[pairNum].fFirstChannel;
    }

    inline UInt_t KTCorrelationData::GetSecondChannel(UInt_t pairNum) const
    {
        return fData[pairNum].fSecondChannel;
    }

    inline UInt_t KTCorrelationData::GetNPairs() const
    {
        return UInt_t(fData.size());
    }

    inline Double_t KTCorrelationData::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline void KTCorrelationData::SetCorrelation(KTFrequencySpectrum* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t pairNum)
    {
        if (pairNum >= fData.size()) fData.resize(pairNum+1);
        fData[pairNum].fCorrelation = record;
        fData[pairNum].fFirstChannel = firstChannel;
        fData[pairNum].fSecondChannel = secondChannel;
    }

    inline void KTCorrelationData::SetNPairs(unsigned pairs)
    {
        fData.resize(pairs);
        return;
    }

    inline void KTCorrelationData::SetTimeInRun(Double_t tir)
    {
        fTimeInRun = tir;
        return;
    }

} /* namespace Katydid */

#endif /* KTCORRELATIONDATA_HH_ */
