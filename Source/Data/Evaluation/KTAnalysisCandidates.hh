/*
 * KTAnalysisCandidates.hh
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#ifndef KTANALYSISCANDIDATES_HH_
#define KTANALYSISCANDIDATES_HH_

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

        public:
            KTAnalysisCandidates();
            virtual ~KTAnalysisCandidates();

            const std::set< Candidate >& GetCandidates() const;
            void AddCandidate(const Candidate& electron);
            void ClearCandidates();

        protected:
            std::set< Candidate > fCandidates;
};

inline const std::set< KTAnalysisCandidates::Candidate >& KTAnalysisCandidates::GetCandidates() const
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
