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
                UInt_t fStartRecord;
                UInt_t fStartSample;
                UInt_t fEndRecord;
                UInt_t fEndSample;

                Candidate(UInt_t startRec, UInt_t startSample, UInt_t endRec, UInt_t endSample)
                {
                    fStartRecord = startRec;
                    fStartSample = startSample;
                    fEndRecord = endRec;
                    fEndSample = endSample;
                }
            };

            struct CandidateCompare
            {
                bool operator() (const Candidate& lhs, const Candidate& rhs)
                {
                    return   lhs.fStartRecord <  rhs.fStartRecord  ||
                            (lhs.fStartRecord == rhs.fStartRecord && lhs.fStartSample <  rhs.fStartSample) ||
                            (lhs.fStartRecord == rhs.fStartRecord && lhs.fStartSample == rhs.fStartSample && lhs.fEndRecord <  rhs.fEndRecord) ||
                            (lhs.fStartRecord == rhs.fStartRecord && lhs.fStartSample == rhs.fStartSample && lhs.fEndRecord == rhs.fEndRecord && lhs.fEndSample < rhs.fEndSample);
                }
            };

            typedef std::set< Candidate, CandidateCompare > CandidateSet;

        public:
            KTAnalysisCandidates();
            virtual ~KTAnalysisCandidates();

            const CandidateSet& GetCandidates() const;
            void AddCandidate(const Candidate& electron);
            void ClearCandidates();

            UInt_t GetRecordSize() const;
            void SetRecordSize(UInt_t size);

        protected:
            CandidateSet fCandidates;

            UInt_t fRecordSize;
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

inline UInt_t KTAnalysisCandidates::GetRecordSize() const
{
    return fRecordSize;
}

inline void KTAnalysisCandidates::SetRecordSize(UInt_t size)
{
    fRecordSize = size;
    return;
}


} /* namespace Katydid */
#endif /* KTANALYSISCANDIDATES_HH_ */
