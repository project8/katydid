/*
 * KTCorrelationData.hh
 *
 *  Created on: Aug 27, 2012
 *      Author: nsoblath
 */

#ifndef KTCORRELATIONDATA_HH_
#define KTCORRELATIONDATA_HH_

#include "KTData.hh"

#include <vector>

namespace Katydid
{
    class KTFrequencySpectrumPolar;

    class KTCorrelationData : public KTExtensibleData< KTCorrelationData >
    {
        protected:
            struct PerPairData
            {
                UInt_t fFirstChannel;
                UInt_t fSecondChannel;
                KTFrequencySpectrumPolar* fCorrelation;
            };

        public:
            KTCorrelationData();
            virtual ~KTCorrelationData();

            const KTFrequencySpectrumPolar* GetCorrelation(UInt_t component = 0) const;
            KTFrequencySpectrumPolar* GetCorrelation(UInt_t component = 0);

            const KTFrequencySpectrum* GetSpectrum(UInt_t component = 0) const;
            KTFrequencySpectrum* GetSpectrum(UInt_t component = 0);

            UInt_t GetFirstChannel(UInt_t component = 0) const;
            UInt_t GetSecondChannel(UInt_t component = 0) const;

            UInt_t GetNComponents() const;

            void SetCorrelation(KTFrequencySpectrumPolar* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t component = 0);

            KTCorrelationData& SetNComponents(unsigned pairs);

        protected:
            std::vector< PerPairData > fData;

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

    inline void KTCorrelationData::SetCorrelation(KTFrequencySpectrumPolar* record, UInt_t firstChannel, UInt_t secondChannel, UInt_t pairNum)
    {
        if (pairNum >= fData.size()) fData.resize(pairNum+1);
        fData[pairNum].fCorrelation = record;
        fData[pairNum].fFirstChannel = firstChannel;
        fData[pairNum].fSecondChannel = secondChannel;
    }

    inline KTCorrelationData& KTCorrelationData::SetNComponents(unsigned pairs)
    {
        fData.resize(pairs);
        return *this;
    }

} /* namespace Katydid */

#endif /* KTCORRELATIONDATA_HH_ */
