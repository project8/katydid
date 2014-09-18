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

            unsigned GetNEvents() const;
            void SetNEvents(unsigned nEvents);

            const std::vector< unsigned >& GetNEventsWithXCandidateMatches() const;
            void SetNEventsWithXCandidateMatches(unsigned xCandMatches, unsigned nEvents);
            void IncrementNEventsWithXCandidateMatches(unsigned xCandidateMathces);
            void ResizeNEventsWithXCandidateMatches(unsigned size);

            unsigned GetNCandidates() const;
            void SetNCandidates(unsigned nCand);

            const std::vector< unsigned >& GetNCandidatesWithXEventMatches() const;
            void SetNCandidatesWithXEventMatches(unsigned xEventMatches, unsigned nCand);
            void IncrementNCandidatesWithXEventMatches(unsigned xEventMatches);
            void ResizeNCandidatesWithXEventMatches(unsigned size);

            double GetEfficiency() const;
            void SetEfficiency(double eff);

            double GetFalseRate() const;
            void SetFalseRate(double rate);

        private:
            double fEventLength;
            double fdfdt;
            double fSignalPower;

            unsigned fNEvents;
            std::vector< unsigned > fNEventsWithXCandidateMatches;

            unsigned fNCandidates;
            std::vector< unsigned > fNCandidatesWithXEventMatches;

            double fEfficiency;
            double fFalseRate;

        public:
            static const std::string sName;
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

    inline unsigned KTCCResults::GetNEvents() const
    {
        return fNEvents;
    }

    inline void KTCCResults::SetNEvents(unsigned nEvents)
    {
        fNEvents = nEvents;
        return;
    }

    inline const std::vector< unsigned >& KTCCResults::GetNEventsWithXCandidateMatches() const
    {
        return fNEventsWithXCandidateMatches;
    }

    inline void KTCCResults::SetNEventsWithXCandidateMatches(unsigned xCandMatches, unsigned nEvents)
    {
        fNEventsWithXCandidateMatches[xCandMatches] = nEvents;
        return;
    }

    inline void KTCCResults::IncrementNEventsWithXCandidateMatches(unsigned xCandMatches)
    {
        fNEventsWithXCandidateMatches[xCandMatches] = fNEventsWithXCandidateMatches[xCandMatches] + 1;
        return;
    }

    inline void KTCCResults::ResizeNEventsWithXCandidateMatches(unsigned size)
    {
        fNEventsWithXCandidateMatches.resize(size);
        return;
    }

    inline unsigned KTCCResults::GetNCandidates() const
    {
        return fNCandidates;
    }

    inline void KTCCResults::SetNCandidates(unsigned nCand)
    {
        fNCandidates = nCand;
        return;
    }

    inline const std::vector< unsigned >& KTCCResults::GetNCandidatesWithXEventMatches() const
    {
        return fNCandidatesWithXEventMatches;
    }

    inline void KTCCResults::SetNCandidatesWithXEventMatches(unsigned xEventMatches, unsigned nCand)
    {
        fNCandidatesWithXEventMatches[xEventMatches] = nCand;
        return;
    }

    inline void KTCCResults::IncrementNCandidatesWithXEventMatches(unsigned xEventMatches)
    {
        fNCandidatesWithXEventMatches[xEventMatches] = fNCandidatesWithXEventMatches[xEventMatches] + 1;
        return;
    }

    inline void KTCCResults::ResizeNCandidatesWithXEventMatches(unsigned size)
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
