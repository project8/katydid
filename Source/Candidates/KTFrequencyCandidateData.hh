/*
 * KTFrequencyCandidateData.hh
 *
 *  Created on: Dec 18, 2012
 *      Author: nsoblath
 */

#ifndef KTFREQUENCYCANDIDATEDATA_HH_
#define KTFREQUENCYCANDIDATEDATA_HH_

#include "KTWriteableData.hh"

#include "KTFrequencyCandidate.hh"

#include "Rtypes.h"

#include <vector>

namespace Katydid
{

    class KTFrequencyCandidateData : public Katydid::KTWriteableData
    {
        public:
            typedef std::vector< KTFrequencyCandidate > Candidates;

        protected:
            struct PerGroupData
            {
                Candidates fCandidates;
                Double_t fThreshold;
            };

        public:
            KTFrequencyCandidateData(UInt_t nGroups=1);
            virtual ~KTFrequencyCandidateData();

            void Accept(KTWriter* writer) const;

            const Candidates& GetCandidates(UInt_t groupNum = 0) const;
            Double_t GetThreshold(UInt_t groupNum = 0) const;
            UInt_t GetNGroups() const;

            void AddCandidate(const KTFrequencyCandidate& candidate, UInt_t groupNum = 0);
            void AddCandidates(const Candidates& candidates, UInt_t groupNum = 0);
            void SetThreshold(Double_t threshold, UInt_t groupNum = 0);
            void SetNGroups(UInt_t channels);

            UInt_t GetNBins() const;
            Double_t GetBinWidth() const;

            void SetNBins(UInt_t nBins);
            void SetBinWidth(Double_t binWidth);

        protected:
            std::vector< PerGroupData > fGroupData;

            UInt_t fNBins;
            Double_t fBinWidth;

    };

    inline const KTFrequencyCandidateData::Candidates& KTFrequencyCandidateData::GetCandidates(UInt_t groupNum) const
    {
        return fGroupData[groupNum].fCandidates;
    }

    inline Double_t KTFrequencyCandidateData::GetThreshold(UInt_t groupNum) const
    {
        return fGroupData[groupNum].fThreshold;
    }

    inline UInt_t KTFrequencyCandidateData::GetNGroups() const
    {
        return UInt_t(fGroupData.size());
    }

    inline void KTFrequencyCandidateData::AddCandidate(const KTFrequencyCandidate& candidate, UInt_t groupNum)
    {
        if (groupNum >= fGroupData.size()) fGroupData.resize(groupNum+1);
        fGroupData[groupNum].fCandidates.push_back(candidate);
        return;
    }

    inline void KTFrequencyCandidateData::AddCandidates(const Candidates& candidates, UInt_t groupNum)
    {
        if (groupNum >= fGroupData.size()) fGroupData.resize(groupNum+1);
        fGroupData[groupNum].fCandidates.insert(fGroupData[groupNum].fCandidates.end(), candidates.begin(), candidates.end());
        return;
    }

    inline void KTFrequencyCandidateData::SetThreshold(Double_t threshold, UInt_t groupNum)
    {
        if (groupNum >= fGroupData.size()) fGroupData.resize(groupNum+1);
        fGroupData[groupNum].fThreshold = threshold;
        return;
    }

    inline void KTFrequencyCandidateData::SetNGroups(UInt_t channels)
    {
        fGroupData.resize(channels);
        return;
    }

    inline UInt_t KTFrequencyCandidateData::GetNBins() const
    {
        return fNBins;
    }

    inline Double_t KTFrequencyCandidateData::GetBinWidth() const
    {
        return fBinWidth;
    }

    inline void KTFrequencyCandidateData::SetNBins(UInt_t nBins)
    {
        fNBins = nBins;
        return;
    }

    inline void KTFrequencyCandidateData::SetBinWidth(Double_t binWidth)
    {
        fBinWidth = binWidth;
        return;
    }


} /* namespace Katydid */
#endif /* KTFREQUENCYCANDIDATEDATA_HH_ */
