/*
 * KTBundleQueueProcessor.hh
 *
 *  Created on: Jan 10, 2013
 *      Author: nsoblath
 */

#ifndef KTBUNDLEQUEUEPROCESSOR_HH_
#define KTBUNDLEQUEUEPROCESSOR_HH_

#include "KTPrimaryProcessor.hh"

#include "KTConcurrentQueue.hh"
#include "KTBundle.hh"
#include "KTLogger.hh"

#include <boost/shared_ptr.hpp>

namespace Katydid
{
    KTLOGGER(eqplog, "katydid.core");

    template< class XProcessorType >
    class KTBundleQueueProcessor : public KTPrimaryProcessor
    {
        public:
            typedef KTConcurrentQueue< boost::shared_ptr<KTBundle> > Queue;

            enum Status
            {
                kStopped,
                kRunning
            };

        public:
            KTBundleQueueProcessor();
            virtual ~KTBundleQueueProcessor();

            Status GetStatus() const;
            void SetStatus(KTBundleQueueProcessor< XProcessorType >::Status);

        protected:
            Status fStatus;

            //**************************************
            // Derived Processor function pointer
            //**************************************
        public:
            void SetFuncPtr(void (XProcessorType::*ptr)(boost::shared_ptr<KTBundle>));

        protected:
            void (XProcessorType::*fFuncPtr)(boost::shared_ptr<KTBundle>);


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
            /// Queue an bundle
            /// Assumes ownership of the bundle
            void QueueBundle(boost::shared_ptr<KTBundle> bundle);


    };

    template< class XProcessorType >
    KTBundleQueueProcessor< XProcessorType >::KTBundleQueueProcessor() :
            KTPrimaryProcessor(),
            fStatus(kStopped),
            fFuncPtr(NULL),
            fQueue()
    {
    }

    template< class XProcessorType >
    KTBundleQueueProcessor< XProcessorType >::~KTBundleQueueProcessor()
    {
    }

    template< class XProcessorType >
    Bool_t KTBundleQueueProcessor< XProcessorType >::Run()
    {
        fStatus = kRunning;
        return ProcessQueue();
    }

    template< class XProcessorType >
    void KTBundleQueueProcessor< XProcessorType >::Stop()
    {
        fStatus = kStopped;
        fQueue.interrupt();
        return;
    }

    template< class XProcessorType >
    void KTBundleQueueProcessor< XProcessorType >::SetFuncPtr(void (XProcessorType::*ptr)(boost::shared_ptr<KTBundle>))
    {
        fFuncPtr = ptr;
        return;
    }


    template< class XProcessorType >
    Bool_t KTBundleQueueProcessor< XProcessorType >::ProcessQueue()
    {
        KTDEBUG(eqplog, "Beginning to process publication queue");
        while (fStatus != kStopped)
        {
            KTDEBUG(eqplog, "processing . . .");
            boost::shared_ptr<KTBundle> bundleToPublish;
            if (fQueue.wait_and_pop(bundleToPublish))
            {
                KTDEBUG(eqplog, "Bundle acquired for publishing");
                fFuncPtr(bundleToPublish);
                if (bundleToPublish->GetIsLastBundle()) fStatus = kStopped;
            }
        }
        return true;
    }

    template< class XProcessorType >
    void KTBundleQueueProcessor< XProcessorType >::ClearQueue()
    {
        while (! fQueue.empty())
        {
            boost::shared_ptr<KTBundle> bundleToDelete;
            fQueue.wait_and_pop(bundleToDelete);
        }
        return;
    }


    template< class XProcessorType >
    void KTBundleQueueProcessor< XProcessorType >::Queue(boost::shared_ptr<KTBundle> bundle)
    {
        KTDEBUG(eqplog, "Queueing bundle");
        fQueue.push(bundle);
        return;
    }


} /* namespace Katydid */
#endif /* KTBUNDLEQUEUEPROCESSOR_HH_ */
