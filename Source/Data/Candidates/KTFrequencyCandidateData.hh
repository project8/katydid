/*
 * KTFrequencyCandidateData.hh
 *
 *  Created on: Dec 18, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYCANDIDATEDATA_HH_
#define KTFREQUENCYCANDIDATEDATA_HH_

#include "KTData.hh"

#include "KTFrequencyCandidate.hh"

#include <vector>

namespace Katydid
{

    class KTFrequencyCandidateData : public KTExtensibleData< KTFrequencyCandidateData >
    {
        public:
            typedef std::vector< KTFrequencyCandidate > Candidates;

        protected:
            struct PerComponentData
            {
                Candidates fCandidates;
                double fThreshold;
            };

        public:
            KTFrequencyCandidateData();
            virtual ~KTFrequencyCandidateData();

            const Candidates& GetCandidates(UInt_t component = 0) const;
            double GetThreshold(UInt_t component = 0) const;

            UInt_t GetNComponents() const;

            void AddCandidate(const KTFrequencyCandidate& candidate, UInt_t component = 0);
            void AddCandidates(const Candidates& candidates, UInt_t component = 0);
            void SetThreshold(double threshold, UInt_t component = 0);

            KTFrequencyCandidateData& SetNComponents(UInt_t components);

            UInt_t GetNBins() const;
            double GetBinWidth() const;

            void SetNBins(UInt_t nBins);
            void SetBinWidth(double binWidth);

        protected:
            std::vector< PerComponentData > fComponentData;

            UInt_t fNBins;
            double fBinWidth;

    };

    inline const KTFrequencyCandidateData::Candidates& KTFrequencyCandidateData::GetCandidates(UInt_t component) const
    {
        return fComponentData[component].fCandidates;
    }

    inline double KTFrequencyCandidateData::GetThreshold(UInt_t component) const
    {
        return fComponentData[component].fThreshold;
    }

    inline UInt_t KTFrequencyCandidateData::GetNComponents() const
    {
        return UInt_t(fComponentData.size());
    }

    inline void KTFrequencyCandidateData::AddCandidate(const KTFrequencyCandidate& candidate, UInt_t component)
    {
        if (component >= fComponentData.size()) SetNComponents(component+1);
        fComponentData[component].fCandidates.push_back(candidate);
        return;
    }

    inline void KTFrequencyCandidateData::AddCandidates(const Candidates& candidates, UInt_t component)
    {
        if (component >= fComponentData.size()) SetNComponents(component+1);
        fComponentData[component].fCandidates.insert(fComponentData[component].fCandidates.end(), candidates.begin(), candidates.end());
        return;
    }

    inline void KTFrequencyCandidateData::SetThreshold(double threshold, UInt_t component)
    {
        if (component >= fComponentData.size()) SetNComponents(component+1);
        fComponentData[component].fThreshold = threshold;
        return;
    }

    inline KTFrequencyCandidateData& KTFrequencyCandidateData::SetNComponents(UInt_t components)
    {
        fComponentData.resize(components);
        return *this;
    }

    inline UInt_t KTFrequencyCandidateData::GetNBins() const
    {
        return fNBins;
    }

    inline double KTFrequencyCandidateData::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTFrequencyCandidateData::SetNBins(UInt_t nBins)
    {
        fNBins = nBins;
        return;
    }

    inline void KTFrequencyCandidateData::SetBinWidth(double binWidth)
    {
        fBinWidth = binWidth;
        return;
    }

} /* namespace Katydid */
#endif /* KTFREQUENCYCANDIDATEDATA_HH_ */
