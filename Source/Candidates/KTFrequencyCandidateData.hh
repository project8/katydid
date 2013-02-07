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

    class KTFrequencyCandidateData : public KTWriteableData
    {
        public:
            typedef std::vector< KTFrequencyCandidate > Candidates;

        protected:
            struct PerComponentData
            {
                Candidates fCandidates;
                Double_t fThreshold;
            };

        public:
            KTFrequencyCandidateData(UInt_t nComponents=1);
            virtual ~KTFrequencyCandidateData();

            void Accept(KTWriter* writer) const;

            const Candidates& GetCandidates(UInt_t groupNum = 0) const;
            Double_t GetThreshold(UInt_t groupNum = 0) const;
            UInt_t GetNComponents() const;

            void AddCandidate(const KTFrequencyCandidate& candidate, UInt_t component = 0);
            void AddCandidates(const Candidates& candidates, UInt_t component = 0);
            void SetThreshold(Double_t threshold, UInt_t groupNum = 0);
            void SetNComponents(UInt_t channels);

            UInt_t GetNBins() const;
            Double_t GetBinWidth() const;
            Double_t GetTimeInRun() const;
            ULong64_t GetSliceNumber() const;

            void SetNBins(UInt_t nBins);
            void SetBinWidth(Double_t binWidth);
            void SetTimeInRun(Double_t tir);
            void SetSliceNumber(ULong64_t slice);

        protected:
            std::vector< PerComponentData > fComponentData;

            UInt_t fNBins;
            Double_t fBinWidth;

            Double_t fTimeInRun;
            ULong64_t fSliceNumber;

    };

    inline const KTFrequencyCandidateData::Candidates& KTFrequencyCandidateData::GetCandidates(UInt_t groupNum) const
    {
        return fComponentData[groupNum].fCandidates;
    }

    inline Double_t KTFrequencyCandidateData::GetThreshold(UInt_t groupNum) const
    {
        return fComponentData[groupNum].fThreshold;
    }

    inline UInt_t KTFrequencyCandidateData::GetNComponents() const
    {
        return UInt_t(fComponentData.size());
    }

    inline void KTFrequencyCandidateData::AddCandidate(const KTFrequencyCandidate& candidate, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fCandidates.push_back(candidate);
        return;
    }

    inline void KTFrequencyCandidateData::AddCandidates(const Candidates& candidates, UInt_t component)
    {
        if (component >= fComponentData.size()) fComponentData.resize(component+1);
        fComponentData[component].fCandidates.insert(fComponentData[component].fCandidates.end(), candidates.begin(), candidates.end());
        return;
    }

    inline void KTFrequencyCandidateData::SetThreshold(Double_t threshold, UInt_t groupNum)
    {
        if (groupNum >= fComponentData.size()) fComponentData.resize(groupNum+1);
        fComponentData[groupNum].fThreshold = threshold;
        return;
    }

    inline void KTFrequencyCandidateData::SetNComponents(UInt_t channels)
    {
        fComponentData.resize(channels);
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

    inline Double_t KTFrequencyCandidateData::GetTimeInRun() const
    {
        return fTimeInRun;
    }

    inline ULong64_t KTFrequencyCandidateData::GetSliceNumber() const
    {
        return fSliceNumber;
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

    inline void KTFrequencyCandidateData::SetTimeInRun(Double_t tir)
    {
        fTimeInRun = tir;
        return;
    }

    inline void KTFrequencyCandidateData::SetSliceNumber(ULong64_t slice)
    {
        fSliceNumber = slice;
        return;
    }


} /* namespace Katydid */
#endif /* KTFREQUENCYCANDIDATEDATA_HH_ */
