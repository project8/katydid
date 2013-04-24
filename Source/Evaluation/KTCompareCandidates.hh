/*
 * KTCompareCandidates.hh
 *
 *  Created on: Apr 9, 2013
 *      Author: nsoblath
 */

#ifndef KTCOMPARECANDIDATES_HH_
#define KTCOMPARECANDIDATES_HH_

#include "KTProcessor.hh"

#include "KTAnalysisCandidates.hh"
#include "KTMCTruthEvents.hh"
#include "KTSlot.hh"

namespace Katydid
{
    class KTPStoreNode;

    class KTCompareCandidates : public KTProcessor
    {
        private:
            // for use in setting up a map keyed by these iterators
            struct CandItCompare
            {
                bool operator() (const KTAnalysisCandidates::CandidateSet::const_iterator& lhs, const KTAnalysisCandidates::CandidateSet::const_iterator& rhs)
                {
                    return lhs->fStartRecord < rhs->fStartRecord ||
                            (lhs->fStartRecord == rhs->fStartRecord && lhs->fStartSample < rhs->fEndSample) ||
                            (lhs->fStartRecord == rhs->fStartRecord && lhs->fStartSample == rhs->fEndSample && lhs->fEndRecord < rhs->fEndRecord) ||
                            (lhs->fStartRecord == rhs->fStartRecord && lhs->fStartSample == rhs->fEndSample && lhs->fEndRecord == rhs->fEndRecord && lhs->fEndSample < rhs->fEndSample);
                }
            };

        public:
            KTCompareCandidates(const std::string& name = "compare-candidates");
            virtual ~KTCompareCandidates();

            Bool_t Configure(const KTPStoreNode* node);

            Bool_t GetAssumeSparseCandidates() const;
            void SetAssumeSparseCandidates(Bool_t flag);

        private:
            Bool_t fAssumeSparseCandidates;

        public:
            Bool_t CompareTruthAndAnalysis(KTMCTruthEvents& mcEventData, KTAnalysisCandidates& candidateData);

        private:
            // Return values:
            //   -1 if candidate occurs completely before event
            //    0 if candidate and event overlap
            //    1 if candidate occurs completely after event
            Int_t CompareAnEventToACandidate(const KTMCTruthEvents::Event& event, const KTAnalysisCandidates::Candidate& candidate, UInt_t eventRecordSize, UInt_t candidateRecordSize) const;

            //***************
            // Slots
            //***************
        private:
            KTSlotDataTwoTypes< KTMCTruthEvents, KTAnalysisCandidates > fTruthAndAnalysisSlot;

    };

    inline Bool_t KTCompareCandidates::GetAssumeSparseCandidates() const
    {
        return fAssumeSparseCandidates;
    }

    inline void KTCompareCandidates::SetAssumeSparseCandidates(Bool_t flag)
    {
        fAssumeSparseCandidates = flag;
        return;
    }


} /* namespace Katydid */
#endif /* KTCOMPARECANDIDATES_HH_ */
