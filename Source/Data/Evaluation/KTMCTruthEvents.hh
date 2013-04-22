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
                UInt_t fStartRecord;
                UInt_t fStartSample;
                UInt_t fEndRecord;
                UInt_t fEndSample;

                Event(UInt_t startRec, UInt_t startSample, UInt_t endRec, UInt_t endSample)
                {
                    fStartRecord = startRec;
                    fStartSample = startSample;
                    fEndRecord = endRec;
                    fEndSample = endSample;
                }
            };

            struct EventCompare
            {
                bool operator() (const Event& lhs, const Event& rhs)
                {
                    return lhs.fStartRecord < rhs.fStartRecord ||
                            (lhs.fStartRecord == rhs.fStartRecord && lhs.fStartSample < rhs.fEndSample) ||
                            (lhs.fStartRecord == rhs.fStartRecord && lhs.fStartSample == rhs.fEndSample && lhs.fEndRecord < rhs.fEndRecord) ||
                            (lhs.fStartRecord == rhs.fStartRecord && lhs.fStartSample == rhs.fEndSample && lhs.fEndRecord == rhs.fEndRecord && lhs.fEndSample < rhs.fEndSample);
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
