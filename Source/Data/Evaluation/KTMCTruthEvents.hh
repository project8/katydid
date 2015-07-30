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
    using namespace Nymph;

    class KTMCTruthEvents : public KTExtensibleData< KTMCTruthEvents >
    {
        public:
            struct Event
            {
                unsigned fStartRecord;
                unsigned fStartSample;
                unsigned fEndRecord;
                unsigned fEndSample;

                Event(unsigned startRec, unsigned startSample, unsigned endRec, unsigned endSample)
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

            double GetEventLength();
            void SetEventLength(double length);

            double Getdfdt();
            void Setdfdt(double dfdt);

            double GetSignalPower();
            void SetSignalPower(double power);

            const EventSet& GetEvents() const;
            void AddEvent(const Event& electron);
            void ClearEvents();

            unsigned GetRecordSize() const;
            void SetRecordSize(unsigned size);

            unsigned GetNRecords() const;
            void SetNRecords(unsigned rec);

        private:
            double fEventLength;
            double fdfdt;
            double fSignalPower;

            EventSet fEvents;

            unsigned fRecordSize;
            unsigned fNRecords;

        public:
            static const std::string sName;
    };

    inline double KTMCTruthEvents::GetEventLength()
    {
        return fEventLength;
    }

    inline void KTMCTruthEvents::SetEventLength(double length)
    {
        fEventLength = length;
        return;
    }

    inline double KTMCTruthEvents::Getdfdt()
    {
        return fdfdt;
    }

    inline void KTMCTruthEvents::Setdfdt(double dfdt)
    {
        fdfdt = dfdt;
        return;
    }

    inline double KTMCTruthEvents::GetSignalPower()
    {
        return fSignalPower;
    }

    inline void KTMCTruthEvents::SetSignalPower(double power)
    {
        fSignalPower = power;
        return;
    }

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

    inline unsigned KTMCTruthEvents::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline void KTMCTruthEvents::SetRecordSize(unsigned size)
    {
        fRecordSize = size;
        return;
    }

    inline unsigned KTMCTruthEvents::GetNRecords() const
    {
        return fNRecords;
    }

    inline void KTMCTruthEvents::SetNRecords(unsigned rec)
    {
        fNRecords = rec;
        return;
    }


} /* namespace Katydid */
#endif /* KTMCTRUTHEVENTS_HH_ */
