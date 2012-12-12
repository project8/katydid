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

    class KTSpectrumDiscriminator : public KTProcessor
    {
        public:
            KTSpectrumDiscriminator();
            virtual ~KTSpectrumDiscriminator();

            Bool_t Configure(const KTPStoreNode* node);

            Double_t GetSNRThreshold() const;
            void SetSNRThreshold(Double_t thresh);

            const std::string& GetInputDataName() const;
            void SetInputDataName(const std::string& name);

            const std::string& GetOutputDataName() const;
            void SetOutputDataName(const std::string& name);

        private:

            Double_t fSNRThreshold;

            std::string fInputDataName;
            std::string fOutputDataName;

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
