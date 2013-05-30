/*
 * KTCCResults.hh
 *
 *  Created on: May 30, 2013
 *      Author: nsoblath
 */

#ifndef KTCCRESULTS_HH_
#define KTCCRESULTS_HH_

#include "KTData.hh"

#include <vector>

namespace Katydid
{

    class KTCCResults : public KTExtensibleData< KTCCResults >
    {
        public:
            KTCCResults();
            virtual ~KTCCResults();

            UInt_t GetNEvents() const;
            void SetNEvents(UInt_t nEvents);

            const std::vector< UInt_t >& GetNEventsWithXCandidateMatches() const;
            void SetNEventsWithXCandidateMatches(UInt_t xCandMatches, UInt_t nEvents);
            void IncrementNEventsWithXCandidateMatches(UInt_t xCandidateMathces);
            void ResizeNEventsWithXCandidateMatches(UInt_t size);

            UInt_t GetNCandidates() const;
            void SetNCandidates(UInt_t nCand);

            const std::vector< UInt_t >& GetNCandidatesWithXEventMatches() const;
            void SetNCandidatesWithXEventMatches(UInt_t xEventMatches, UInt_t nCand);
            void IncrementNCandidatesWithXEventMatches(UInt_t xEventMatches);
            void ResizeNCandidatesWithXEventMatches(UInt_t size);

            Double_t GetEfficiency() const;
            void SetEfficiency(Double_t eff);

            Double_t GetFalseRate() const;
            void SetFalseRate(Double_t rate);

        private:
            UInt_t fNEvents;
            std::vector< UInt_t > fNEventsWithXCandidateMatches;

            UInt_t fNCandidates;
            std::vector< UInt_t > fNCandidatesWithXEventMatches;

            Double_t fEfficiency;
            Double_t fFalseRate;
    };

    inline UInt_t KTCCResults::GetNEvents() const
    {
        return fNEvents;
    }

    inline void KTCCResults::SetNEvents(UInt_t nEvents)
    {
        fNEvents = nEvents;
        return;
    }

    inline const std::vector< UInt_t >& KTCCResults::GetNEventsWithXCandidateMatches() const
    {
        return fNEventsWithXCandidateMatches;
    }

    inline void KTCCResults::SetNEventsWithXCandidateMatches(UInt_t xCandMatches, UInt_t nEvents)
    {
        fNEventsWithXCandidateMatches[xCandMatches] = nEvents;
        return;
    }

    inline void KTCCResults::IncrementNEventsWithXCandidateMatches(UInt_t xCandMatches)
    {
        fNEventsWithXCandidateMatches[xCandMatches] = fNEventsWithXCandidateMatches[xCandMatches] + 1;
        return;
    }

    inline void KTCCResults::ResizeNEventsWithXCandidateMatches(UInt_t size)
    {
        fNEventsWithXCandidateMatches.resize(size);
        return;
    }

    inline UInt_t KTCCResults::GetNCandidates() const
    {
        return fNCandidates;
    }

    inline void KTCCResults::SetNCandidates(UInt_t nCand)
    {
        fNCandidates = nCand;
        return;
    }

    inline const std::vector< UInt_t >& KTCCResults::GetNCandidatesWithXEventMatches() const
    {
        return fNCandidatesWithXEventMatches;
    }

    inline void KTCCResults::SetNCandidatesWithXEventMatches(UInt_t xEventMatches, UInt_t nCand)
    {
        fNCandidatesWithXEventMatches[xEventMatches] = nCand;
        return;
    }

    inline void KTCCResults::IncrementNCandidatesWithXEventMatches(UInt_t xEventMatches)
    {
        fNCandidatesWithXEventMatches[xEventMatches] = fNCandidatesWithXEventMatches[xEventMatches] + 1;
        return;
    }

    inline void KTCCResults::ResizeNCandidatesWithXEventMatches(UInt_t size)
    {
        fNCandidatesWithXEventMatches.resize(size);
        return;
    }

    inline Double_t KTCCResults::GetEfficiency() const
    {
        return fEfficiency;
    }

    inline void KTCCResults::SetEfficiency(Double_t eff)
    {
        fEfficiency = eff;
        return;
    }

    inline Double_t KTCCResults::GetFalseRate() const
    {
        return fFalseRate;
    }

    inline void KTCCResults::SetFalseRate(Double_t rate)
    {
        fFalseRate = rate;
        return;
    }


} /* namespace Katydid */
#endif /* KTCCRESULTS_HH_ */
