/*
 * KTCorrelationData.hh
 *
 *  Created on: Aug 27, 2012
 *      Author: nsoblath
 */

#ifndef KTCORRELATIONDATA_HH_
#define KTCORRELATIONDATA_HH_

#include "KTFrequencySpectrumData.hh"

#include <vector>

namespace Katydid
{
    class KTFrequencySpectrumPolar;

    class KTCorrelationData : public KTFrequencySpectrumData
    {
        protected:
            struct PerPairData
            {
                UInt_t fFirstChannel;
                UInt_t fSecondChannel;
                KTFrequencySpectrumPolar* fCorrelation;
            };

        public:
            KTCorrelationData(unsigned nChannels=1);
            virtual ~KTCorrelationData();

            const KTFrequencySpectrumPolar* GetCorrelation(UInt_t pairNum = 0) const;
            KTFrequencySpectrumPolar* GetCorrelation(UInt_t pairNum = 0);

            virtual const KTFrequencySpectrum* GetSpectrum(UInt_t pairNum = 0) const;
            virtual KTFrequencySpectrum* GetSpectrum(UInt_t pairNum = 0);
            UInt_t GetFirstChannel(UInt_t pairNum = 0) const;
            UInt_t GetSecondChannel(UInt_t pairNum = 0) const;
            virtual UInt_t GetNComponents() const;
            virtual Double_t GetTimeInRun() const;
            virtual Double_t GetTimeLength() const;
            virtual ULong64_t GetSliceNumber() const;

            void SetCorrelation(KTFrequencySpectrumPolar* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t pairNum = 0);
            virtual void SetNComponents(unsigned pairs);
            virtual void SetTimeInRun(Double_t tir);
            virtual void SetTimeLength(Double_t length);
            virtual void SetSliceNumber(ULong64_t slice);

            virtual void Accept(KTWriter* writer) const;

        protected:
            std::vector< PerPairData > fData;

            Double_t fTimeInRun;
            Double_t fLength;
            ULong64_t fSliceNumber;
    };

    inline const KTFrequencySpectrumPolar* KTCorrelationData::GetCorrelation(UInt_t pairNum) const
    {
        return fData[pairNum].fCorrelation;
    }

    inline KTFrequencySpectrumPolar* KTCorrelationData::GetCorrelation(UInt_t pairNum)
    {
        return fData[pairNum].fCorrelation;
    }

    inline const KTFrequencySpectrum* KTCorrelationData::GetSpectrum(UInt_t pairNum) const
    {
        return fData[pairNum].fCorrelation;
    }

    inline KTFrequencySpectrum* KTCorrelationData::GetSpectrum(UInt_t pairNum)
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

    inline UInt_t KTCorrelationData::GetNComponents() const
    {
        return UInt_t(fData.size());
    }

    inline Double_t KTCorrelationData::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline Double_t KTCorrelationData::GetTimeLength() const
    {
        return fLength;
    }

    inline ULong64_t KTCorrelationData::GetSliceNumber() const
    {
        return fSliceNumber;
    }

    inline void KTCorrelationData::SetCorrelation(KTFrequencySpectrumPolar* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t pairNum)
    {
        if (pairNum >= fData.size()) fData.resize(pairNum+1);
        fData[pairNum].fCorrelation = record;
        fData[pairNum].fFirstChannel = firstChannel;
        fData[pairNum].fSecondChannel = secondChannel;
    }

    inline void KTCorrelationData::SetNComponents(unsigned pairs)
    {
        fData.resize(pairs);
        return;
    }

    inline void KTCorrelationData::SetTimeInRun(Double_t tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTCorrelationData::SetTimeLength(Double_t length)
    {
        fLength = length;
        return;
    }

    inline void KTCorrelationData::SetSliceNumber(ULong64_t slice)
    {
        fSliceNumber = slice;
        return;
    }

} /* namespace Katydid */

#endif /* KTCORRELATIONDATA_HH_ */
