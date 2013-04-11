/*
 * KTAnalysisCandidates.hh
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#ifndef KTANALYSISCANDIDATES_HH_
#define KTANALYSISCANDIDATES_HH_

#include "KTData.hh"

#include <set>

namespace Katydid
{

    class KTAnalysisCandidates : public KTExtensibleData< KTAnalysisCandidates >
    {
        public:
            struct Candidate
            {
                Double_t fStartTime;
                Double_t fEndTime;
                Double_t fStartTimeUncert;
                Double_t fEndTimeUncert;

                Candidate(Double_t startTime, Double_t endTime, Double_t startTimeUncert, Double_t endTimeUncert)
                {
                    fStartTime = startTime;
                    fEndTime = endTime;
                    fStartTimeUncert = startTimeUncert;
                    fEndTimeUncert = endTimeUncert;
                }
            };

            struct CandidateCompare
            {
                bool operator() (const Candidate& lhs, const Candidate& rhs)
                {
                    return lhs.fStartTime < rhs.fStartTime || (lhs.fStartTime == rhs.fStartTime && lhs.fEndTime < rhs.fEndTime);
                }
            };

            typedef std::set< Candidate, CandidateCompare > CandidateSet;

        public:
            KTAnalysisCandidates();
            virtual ~KTAnalysisCandidates();

            const CandidateSet& GetCandidates() const;
            void AddCandidate(const Candidate& electron);
            void ClearCandidates();

        protected:
            CandidateSet fCandidates;
};

inline const KTAnalysisCandidates::CandidateSet& KTAnalysisCandidates::GetCandidates() const
{
    return fCandidates;
}

inline void KTAnalysisCandidates::AddCandidate(const Candidate& candidate)
{
    fCandidates.insert(candidate);
    return;
}

inline void KTAnalysisCandidates::ClearCandidates()
{
    fCandidates.clear();
    return;
}


} /* namespace Katydid */
#endif /* KTANALYSISCANDIDATES_HH_ */
