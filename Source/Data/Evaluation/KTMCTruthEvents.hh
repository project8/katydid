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

            Double_t GetEventLength();
            void SetEventLength(Double_t length);

            Double_t Getdfdt();
            void Setdfdt(Double_t dfdt);

            Double_t GetSignalPower();
            void SetSignalPower(Double_t power);

            const EventSet& GetEvents() const;
            void AddEvent(const Event& electron);
            void ClearEvents();

            UInt_t GetRecordSize() const;
            void SetRecordSize(UInt_t size);

            UInt_t GetNRecords() const;
            void SetNRecords(UInt_t rec);

        protected:
            Double_t fEventLength;
            Double_t fdfdt;
            Double_t fSignalPower;

            EventSet fEvents;

            UInt_t fRecordSize;
            UInt_t fNRecords;
    };

    inline Double_t KTMCTruthEvents::GetEventLength()
    {
        return fEventLength;
    }

    inline void KTMCTruthEvents::SetEventLength(Double_t length)
    {
        fEventLength = length;
        return;
    }

    inline Double_t KTMCTruthEvents::Getdfdt()
    {
        return fdfdt;
    }

    inline void KTMCTruthEvents::Setdfdt(Double_t dfdt)
    {
        fdfdt = dfdt;
        return;
    }

    inline Double_t KTMCTruthEvents::GetSignalPower()
    {
        return fSignalPower;
    }

    inline void KTMCTruthEvents::SetSignalPower(Double_t power)
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

    inline UInt_t KTMCTruthEvents::GetRecordSize() const
    {
        return fRecordSize;
    }

    inline void KTMCTruthEvents::SetRecordSize(UInt_t size)
    {
        fRecordSize = size;
        return;
    }

    inline UInt_t KTMCTruthEvents::GetNRecords() const
    {
        return fNRecords;
    }

    inline void KTMCTruthEvents::SetNRecords(UInt_t rec)
    {
        fNRecords = rec;
        return;
    }


} /* namespace Katydid */
#endif /* KTMCTRUTHEVENTS_HH_ */
