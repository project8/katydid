/*
 * KTMCTruthEvents.hh
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#ifndef KTMCTRUTHEVENTS_HH_
#define KTMCTRUTHEVENTS_HH_

#include "KTData.hh"

#include <set>

namespace Katydid
{

    class KTMCTruthEvents : public KTExtensibleData< KTMCTruthEvents >
    {
        public:
            struct Event
            {
                Double_t fStartTime;
                Double_t fEndTime;

                Event(Double_t startTime, Double_t endTime)
                {
                    fStartTime = startTime;
                    fEndTime = endTime;
                }
            };

            struct EventCompare
            {
                bool operator() (const Event& lhs, const Event& rhs)
                {
                    return lhs.fStartTime < rhs.fStartTime || (lhs.fStartTime == rhs.fStartTime && lhs.fEndTime < rhs.fEndTime);
                }
            };

            typedef std::set< Event, EventCompare > EventSet;

        public:
            KTMCTruthEvents();
            virtual ~KTMCTruthEvents();

            const EventSet& GetEvents() const;
            void AddEvent(const Event& electron);
            void ClearEvents();

        protected:
            EventSet fEvents;
    };

    inline const KTMCTruthEvents::EventSet& KTMCTruthEvents::GetEvents() const
    {
        return fEvents;
    }

    inline void KTMCTruthEvents::AddEvent(const Event& electron)
    {
        fEvents.insert(electron);
        return;
    }

    inline void KTMCTruthEvents::ClearEvents()
    {
        fEvents.clear();
        return;
    }

} /* namespace Katydid */
#endif /* KTMCTRUTHEVENTS_HH_ */
