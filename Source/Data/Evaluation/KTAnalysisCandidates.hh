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
                unsigned fStartRecord;
                unsigned fStartSample;
                unsigned fEndRecord;
                unsigned fEndSample;

                Candidate(unsigned startRec, unsigned startSample, unsigned endRec, unsigned endSample)
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

            unsigned GetRecordSize() const;
            void SetRecordSize(unsigned size);

            unsigned GetNRecords() const;
            void SetNRecords(unsigned rec);

        private:
            CandidateSet fCandidates;

            unsigned fRecordSize;
            unsigned fNRecords;

            static const std::string sName;
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

    inline unsigned KTAnalysisCandidates::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline void KTAnalysisCandidates::SetRecordSize(unsigned size)
    {
        fRecordSize = size;
        return;
    }

    inline unsigned KTAnalysisCandidates::GetNRecords() const
    {
        return fNRecords;
    }

    inline void KTAnalysisCandidates::SetNRecords(unsigned rec)
    {
        fNRecords = rec;
        return;
    }


} /* namespace Katydid */
#endif /* KTANALYSISCANDIDATES_HH_ */
