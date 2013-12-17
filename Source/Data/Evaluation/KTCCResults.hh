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

            double GetEventLength();
            void SetEventLength(double length);

            double Getdfdt();
            void Setdfdt(double dfdt);

            double GetSignalPower();
            void SetSignalPower(double power);

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

            double GetEfficiency() const;
            void SetEfficiency(double eff);

            double GetFalseRate() const;
            void SetFalseRate(double rate);

        private:
            double fEventLength;
            double fdfdt;
            double fSignalPower;

            UInt_t fNEvents;
            std::vector< UInt_t > fNEventsWithXCandidateMatches;

            UInt_t fNCandidates;
            std::vector< UInt_t > fNCandidatesWithXEventMatches;

            double fEfficiency;
            double fFalseRate;
    };

    inline double KTCCResults::GetEventLength()
    {
        return fEventLength;
    }

    inline void KTCCResults::SetEventLength(double length)
    {
        fEventLength = length;
        return;
    }

    inline double KTCCResults::Getdfdt()
    {
        return fdfdt;
    }

    inline void KTCCResults::Setdfdt(double dfdt)
    {
        fdfdt = dfdt;
        return;
    }

    inline double KTCCResults::GetSignalPower()
    {
        return fSignalPower;
    }

    inline void KTCCResults::SetSignalPower(double power)
    {
        fSignalPower = power;
        return;
    }

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

    inline double KTCCResults::GetEfficiency() const
    {
        return fEfficiency;
    }

    inline void KTCCResults::SetEfficiency(double eff)
    {
        fEfficiency = eff;
        return;
    }

    inline double KTCCResults::GetFalseRate() const
    {
        return fFalseRate;
    }

    inline void KTCCResults::SetFalseRate(double rate)
    {
        fFalseRate = rate;
        return;
    }


} /* namespace Katydid */
#endif /* KTCCRESULTS_HH_ */
