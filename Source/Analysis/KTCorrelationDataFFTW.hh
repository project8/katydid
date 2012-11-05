/*
 * KTCorrelationDataFFTW.hh
 *
 *  Created on: Aug 27, 2012
 *      Author: nsoblath
 */

#ifndef KTCORRELATIONDATAFFTW_HH_
#define KTCORRELATIONDATAFFTW_HH_

#include "KTWriteableData.hh"

#include <vector>

namespace Katydid
{
    class KTFrequencySpectrumFFTW;

    class KTCorrelationDataFFTW : public KTWriteableData
    {
        protected:
            struct PerPairData
            {
                UInt_t fFirstChannel;
                UInt_t fSecondChannel;
                KTFrequencySpectrumFFTW* fCorrelation;
            };

        public:
            KTCorrelationDataFFTW(unsigned nChannels=1);
            virtual ~KTCorrelationDataFFTW();

            const std::string& GetName() const;
            static const std::string& StaticGetName();

            const KTFrequencySpectrumFFTW* GetCorrelation(UInt_t pairNum = 0) const;
            KTFrequencySpectrumFFTW* GetCorrelation(UInt_t pairNum = 0);
            UInt_t GetFirstChannel(UInt_t pairNum = 0) const;
            UInt_t GetSecondChannel(UInt_t pairNum = 0) const;
            UInt_t GetNPairs() const;

            void SetCorrelation(KTFrequencySpectrumFFTW* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t pairNum = 0);
            void SetNPairs(unsigned pairs);

            void Accept(KTWriter* writer) const;

        protected:
            static std::string fName;

            std::vector< PerPairData > fData;

    };

    inline const std::string& KTCorrelationDataFFTW::GetName() const
    {
        return fName;
    }

    inline const KTFrequencySpectrumFFTW* KTCorrelationDataFFTW::GetCorrelation(UInt_t pairNum) const
    {
        return fData[pairNum].fCorrelation;
    }

    inline KTFrequencySpectrumFFTW* KTCorrelationDataFFTW::GetCorrelation(UInt_t pairNum)
    {
        return fData[pairNum].fCorrelation;
    }

    inline UInt_t KTCorrelationDataFFTW::GetFirstChannel(UInt_t pairNum) const
    {
        return fData[pairNum].fFirstChannel;
    }

    inline UInt_t KTCorrelationDataFFTW::GetSecondChannel(UInt_t pairNum) const
    {
        return fData[pairNum].fSecondChannel;
    }

    inline UInt_t KTCorrelationDataFFTW::GetNPairs() const
    {
        return UInt_t(fData.size());
    }

    inline void KTCorrelationDataFFTW::SetCorrelation(KTFrequencySpectrumFFTW* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t pairNum)
    {
        if (pairNum >= fData.size()) fData.resize(pairNum+1);
        fData[pairNum].fCorrelation = record;
        fData[pairNum].fFirstChannel = firstChannel;
        fData[pairNum].fSecondChannel = secondChannel;
    }

    inline void KTCorrelationDataFFTW::SetNPairs(unsigned pairs)
    {
        fData.resize(pairs);
        return;
    }

} /* namespace Katydid */

#endif /* KTCORRELATIONDATAFFTW_HH_ */
