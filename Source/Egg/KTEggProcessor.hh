/**
 @file KTEggProcessor.hh
 @brief Contains KTEggProcessor
 @details Iterates over events in an Egg file.
 @author: N. S. Oblath
 @date: Jan 5, 2012
 */

#ifndef KTEGGPROCESSOR_HH_
#define KTEGGPROCESSOR_HH_

#include "KTProcessor.hh"

#include "KTEgg.hh"

#include "Rtypes.h"

#include <boost/signals2.hpp>

#include <string>

namespace Katydid
{
    class KTEvent;

    /*!
     @class KTEggProcessor
     @author N. S. Oblath

     @brief Iterates over the events in an Egg file.

     @details
     Iterates over events in an egg file; events are extracted until fNEvents is reached.

     A signal is emitted when the header is parsed.  The signature for the signal call is void (KTEgg::HeaderInfo info).
     A signal is emitted for each event extracted.  The signature for the signal call is void (UInt_t iEvent, const KTEvent* eventPtr).
    */
    class KTEggProcessor : public KTProcessor
    {
        public:
            typedef boost::signals2::signal< void (KTEgg::HeaderInfo) > HeaderSignal;
            typedef boost::signals2::signal< void (UInt_t, const KTEvent*) > EventSignal;

        public:
            KTEggProcessor();
            virtual ~KTEggProcessor();

            Bool_t ApplySetting(const KTSetting* setting);

            Bool_t ProcessEgg(const std::string& fileName);

            UInt_t GetNEvents() const;
            void SetNEvents(UInt_t nEvents);

        private:
            UInt_t fNEvents;

            //***************
            // Signals
            //***************

        public:
            boost::signals2::connection ConnectToHeaderSignal(const HeaderSignal::slot_type &subscriber);
            boost::signals2::connection ConnectToEventSignal(const EventSignal::slot_type &subscriber);

        private:
            HeaderSignal fHeaderSignal;
            EventSignal fEventSignal;

    };

    inline UInt_t KTEggProcessor::GetNEvents() const
    {
        return fNEvents;
    }

    inline void KTEggProcessor::SetNEvents(UInt_t nEvents)
    {
        fNEvents = nEvents;
        return;
    }

    inline boost::signals2::connection KTEggProcessor::ConnectToHeaderSignal(const HeaderSignal::slot_type &subscriber)
    {
        return fHeaderSignal.connect(subscriber);
    }

    inline boost::signals2::connection KTEggProcessor::ConnectToEventSignal(const EventSignal::slot_type &subscriber)
    {
        return fEventSignal.connect(subscriber);
    }

} /* namespace Katydid */
#endif /* KTEGGPROCESSOR_HH_ */
