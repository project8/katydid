/*
 * KTEventQueueProcessor.hh
 *
 *  Created on: Jan 10, 2013
 *      Author: nsoblath
 */

#ifndef KTEVENTQUEUEPROCESSOR_HH_
#define KTEVENTQUEUEPROCESSOR_HH_

#include "KTPrimaryProcessor.hh"

#include "KTConcurrentQueue.hh"
#include "KTEvent.hh"
#include "KTLogger.hh"

#include <boost/shared_ptr.hpp>

namespace Katydid
{
    KTLOGGER(eqplog, "katydid.core");

    template< class XProcessorType >
    class KTEventQueueProcessor : public KTPrimaryProcessor
    {
        public:
            typedef KTConcurrentQueue< boost::shared_ptr<KTEvent> > Queue;

            enum Status
            {
                kStopped,
                kRunning
            };

        public:
            KTEventQueueProcessor();
            virtual ~KTEventQueueProcessor();

            Status GetStatus() const;
            void SetStatus(KTEventQueueProcessor< XProcessorType >::Status);

        protected:
            Status fStatus;

            //**************************************
            // Derived Processor function pointer
            //**************************************
        public:
            void SetFuncPtr(void (XProcessorType::*ptr)(boost::shared_ptr<KTEvent>));

        protected:
            void (XProcessorType::*fFuncPtr)(boost::shared_ptr<KTEvent>);


            //*********
            // Queue
            //*********
        public:
            /// Begins processing of queue (switches status from kStopped to kRunning)
            Bool_t Run();

            /// Stops processing of queue (switches status to kStopped)
            void Stop();

            /// Begins processing of queue if status is already kRunning; otherwise does nothing.
            Bool_t ProcessQueue();

            void ClearQueue();

        protected:
            Queue fQueue;

            //*********
            // Slots
            //*********
        public:
            /// Queue an event
            /// Assumes ownership of the event
            void QueueEvent(boost::shared_ptr<KTEvent> event);


    };

    template< class XProcessorType >
    KTEventQueueProcessor< XProcessorType >::KTEventQueueProcessor() :
            KTPrimaryProcessor(),
            fStatus(kStopped),
            fFuncPtr(NULL),
            fQueue()
    {
    }

    template< class XProcessorType >
    KTEventQueueProcessor< XProcessorType >::~KTEventQueueProcessor()
    {
    }

    template< class XProcessorType >
    Bool_t KTEventQueueProcessor< XProcessorType >::Run()
    {
        fStatus = kRunning;
        return ProcessQueue();
    }

    template< class XProcessorType >
    void KTEventQueueProcessor< XProcessorType >::Stop()
    {
        fStatus = kStopped;
        fQueue.interrupt();
        return;
    }

    template< class XProcessorType >
    void KTEventQueueProcessor< XProcessorType >::SetFuncPtr(void (XProcessorType::*ptr)(boost::shared_ptr<KTEvent>))
    {
        fFuncPtr = ptr;
        return;
    }


    template< class XProcessorType >
    Bool_t KTEventQueueProcessor< XProcessorType >::ProcessQueue()
    {
        KTDEBUG(eqplog, "Beginning to process publication queue");
        while (fStatus != kStopped)
        {
            KTDEBUG(eqplog, "processing . . .");
            boost::shared_ptr<KTEvent> eventToPublish;
            if (fQueue.wait_and_pop(eventToPublish))
            {
                KTDEBUG(eqplog, "Event acquired for publishing");
                fFuncPtr(eventToPublish);
                if (eventToPublish->GetIsLastEvent()) fStatus = kStopped;
            }
        }
        return true;
    }

    template< class XProcessorType >
    void KTEventQueueProcessor< XProcessorType >::ClearQueue()
    {
        while (! fQueue.empty())
        {
            boost::shared_ptr<KTEvent> eventToDelete;
            fQueue.wait_and_pop(eventToDelete);
        }
        return;
    }


    template< class XProcessorType >
    void KTEventQueueProcessor< XProcessorType >::Queue(boost::shared_ptr<KTEvent> event)
    {
        KTDEBUG(eqplog, "Queueing event");
        fQueue.push(event);
        return;
    }


} /* namespace Katydid */
#endif /* KTEVENTQUEUEPROCESSOR_HH_ */
