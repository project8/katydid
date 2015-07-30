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
    using namespace Nymph;

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

            const Candidates& GetCandidates(unsigned component = 0) const;
            double GetThreshold(unsigned component = 0) const;

            unsigned GetNComponents() const;

            void AddCandidate(const KTFrequencyCandidate& candidate, unsigned component = 0);
            void AddCandidates(const Candidates& candidates, unsigned component = 0);
            void SetThreshold(double threshold, unsigned component = 0);

            KTFrequencyCandidateData& SetNComponents(unsigned components);

            unsigned GetNBins() const;
            double GetBinWidth() const;

            void SetNBins(unsigned nBins);
            void SetBinWidth(double binWidth);

        private:
            std::vector< PerComponentData > fComponentData;

            unsigned fNBins;
            double fBinWidth;

        public:
            static const std::string sName;
    };

    inline const KTFrequencyCandidateData::Candidates& KTFrequencyCandidateData::GetCandidates(unsigned component) const
    {
        return fComponentData[component].fCandidates;
    }

    inline double KTFrequencyCandidateData::GetThreshold(unsigned component) const
    {
        return fComponentData[component].fThreshold;
    }

    inline unsigned KTFrequencyCandidateData::GetNComponents() const
    {
        return unsigned(fComponentData.size());
    }

    inline void KTFrequencyCandidateData::AddCandidate(const KTFrequencyCandidate& candidate, unsigned component)
    {
        if (component >= fComponentData.size()) SetNComponents(component+1);
        fComponentData[component].fCandidates.push_back(candidate);
        return;
    }

    inline void KTFrequencyCandidateData::AddCandidates(const Candidates& candidates, unsigned component)
    {
        if (component >= fComponentData.size()) SetNComponents(component+1);
        fComponentData[component].fCandidates.insert(fComponentData[component].fCandidates.end(), candidates.begin(), candidates.end());
        return;
    }

    inline void KTFrequencyCandidateData::SetThreshold(double threshold, unsigned component)
    {
        if (component >= fComponentData.size()) SetNComponents(component+1);
        fComponentData[component].fThreshold = threshold;
        return;
    }

    inline KTFrequencyCandidateData& KTFrequencyCandidateData::SetNComponents(unsigned components)
    {
        fComponentData.resize(components);
        return *this;
    }

    inline unsigned KTFrequencyCandidateData::GetNBins() const
    {
        return fNBins;
    }

    inline double KTFrequencyCandidateData::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTFrequencyCandidateData::SetNBins(unsigned nBins)
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
