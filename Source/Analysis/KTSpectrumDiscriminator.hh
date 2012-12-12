/*
 * KTSpectrumDiscriminator.hh
 *
 *  Created on: Dec 12, 2012
 *      Author: nsoblath
 */

#ifndef KTSPECTRUMDISCRIMINATOR_HH_
#define KTSPECTRUMDISCRIMINATOR_HH_

#include "KTProcessor.hh"




namespace Katydid
{
    class KTDiscriminatedPoints1DData;
    class KTFrequencySpectrumData;

    class KTSpectrumDiscriminator : public KTProcessor
    {
        public:
            KTSpectrumDiscriminator();
            virtual ~KTSpectrumDiscriminator();

            Bool_t Configure(const KTPStoreNode* node);

            Double_t GetSNRThreshold() const;
            void SetSNRThreshold(Double_t thresh);

            Double_t GetMinFrequency() const;
            void SetMinFrequency(Double_t freq);

            Double_t GetMaxFrequency() const;
            void SetMaxFrequency(Double_t freq);

            UInt_t GetMinBin() const;
            void SetMinBin(UInt_t bin);

            UInt_t GetMaxBin() const;
            void SetMaxBin(UInt_t bin);

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        private:

            Double_t fSNRThreshold;

            Double_t fMinFrequency;
            Double_t fMaxFrequency;
            UInt_t fMinBin;
            UInt_t fMaxBin;
            Bool_t fCalculateMinBin;
            Bool_t fCalculateMaxBin;

            std::string fInputDataName;
            std::string fOutputDataName;

        public:
            KTDiscriminatedPoints1DData* Discriminate(const KTFrequencySpectrumData* data);

    };

    inline Double_t KTSpectrumDiscriminator::GetSNRThreshold() const
    {
        return fSNRThreshold;
    }

    inline void KTSpectrumDiscriminator::SetSNRThreshold(Double_t thresh)
    {
        fSNRThreshold = thresh;
        return;
    }

    inline Double_t KTSpectrumDiscriminator::GetMaxFrequency() const
    {
        return fMaxFrequency;
    }

    inline void KTSpectrumDiscriminator::SetMaxFrequency(Double_t freq)
    {
        fMaxFrequency = freq;
        fCalculateMaxBin = true;
        return;
    }

    inline UInt_t KTSpectrumDiscriminator::GetMinBin() const
    {
        return fMinBin;
    }

    inline void KTSpectrumDiscriminator::SetMinBin(UInt_t bin)
    {
        fMinBin = bin;
        fCalculateMinBin = false;
        return;
    }

    inline const std::string& KTSpectrumDiscriminator::GetInputDataName() const
    {
        return fInputDataName;
    }

    inline void KTSpectrumDiscriminator::SetInputDataName(const std::string& name)
    {
        fInputDataName = name;
        return;
    }

    inline const std::string& KTSpectrumDiscriminator::GetOutputDataName() const
    {
        return fOutputDataName;
    }

    inline void KTSpectrumDiscriminator::SetOutputDataName(const std::string& name)
    {
        fOutputDataName = name;
        return;
    }

} /* namespace Katydid */
#endif /* KTSPECTRUMDISCRIMINATOR_HH_ */
