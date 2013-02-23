/*
 * KTBundleQueueProcessorTemplate.hh
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

    //***********************************
    // Bundle Queue Processor Template
    //***********************************

    template< class XProcessorType >
    class KTBundleQueueProcessorTemplate : public KTPrimaryProcessor
    {
        public:
            typedef KTConcurrentQueue< boost::shared_ptr<KTBundle> > Queue;

            enum Status
            {
                kStopped,
                kRunning
            };

        public:
            KTBundleQueueProcessorTemplate();
            virtual ~KTBundleQueueProcessorTemplate();

            Status GetStatus() const;
            void SetStatus(KTBundleQueueProcessorTemplate< XProcessorType >::Status);

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

            /// Queue a list of bundles
            /// Assumes ownership of all bundles and the list
            void QueueBundles(std::list< boost::shared_ptr<KTBundle> >* bundleList);
    };


    //**************************
    // Bundle Queue Processor
    //**************************

    class KTBundleQueueProcessor : public KTBundleQueueProcessorTemplate< KTBundleQueueProcessor >
    {
        protected:
            typedef KTSignal< void (boost::shared_ptr<KTBundle>) >::signal BundleSignal;

        public:
            KTBundleQueueProcessor();
            virtual ~KTBundleQueueProcessor();

            Bool_t Configure(const KTPStoreNode* node);

        public:
            void EmitBundleSignal(boost::shared_ptr<KTBundle> bundle);

            //***************
            // Signals
            //***************

        private:
            BundleSignal fBundleSignal;

            //*********
            // Slots
            //*********
        //public:
            // QueueBundle from KTBundleQueueProcessorTemplate
            // QueueBundles from KTBundleQueueProcessorTemplate

    };


    //**************************************************
    // Bundle Queue Processor Template Implementation
    //**************************************************


    template< class XProcessorType >
    KTBundleQueueProcessorTemplate< XProcessorType >::KTBundleQueueProcessorTemplate() :
            KTPrimaryProcessor(),
            fStatus(kStopped),
            fFuncPtr(NULL),
            fQueue()
    {
            RegisterSlot("bundle", this, &KTBundleQueueProcessorTemplate< XProcessorType >::QueueBundle, "void (shared_ptr<KTBundle>)");
            RegisterSlot("bundle-list", this, &KTBundleQueueProcessorTemplate< XProcessorType >::QueueBundles, "void (list< shared_ptr<KTBundle> >)");
    }

    template< class XProcessorType >
    KTBundleQueueProcessorTemplate< XProcessorType >::~KTBundleQueueProcessorTemplate()
    {
        ClearQueue();
    }

    template< class XProcessorType >
    Bool_t KTBundleQueueProcessorTemplate< XProcessorType >::Run()
    {
        fStatus = kRunning;
        return ProcessQueue();
    }

    template< class XProcessorType >
    void KTBundleQueueProcessorTemplate< XProcessorType >::Stop()
    {
        fStatus = kStopped;
        fQueue.interrupt();
        return;
    }

    template< class XProcessorType >
    void KTBundleQueueProcessorTemplate< XProcessorType >::SetFuncPtr(void (XProcessorType::*ptr)(boost::shared_ptr<KTBundle>))
    {
        fFuncPtr = ptr;
        return;
    }


    template< class XProcessorType >
    Bool_t KTBundleQueueProcessorTemplate< XProcessorType >::ProcessQueue()
    {
        KTDEBUG(eqplog, "Beginning to process publication queue");
        while (fStatus != kStopped)
        {
            KTDEBUG(eqplog, "processing . . .");
            boost::shared_ptr<KTBundle> bundleToPublish;
            if (fQueue.wait_and_pop(bundleToPublish))
            {
                KTDEBUG(eqplog, "Bundle acquired for publishing");
                (static_cast<XProcessorType*>(this)->*fFuncPtr)(bundleToPublish);
                if (bundleToPublish->GetIsLastBundle()) fStatus = kStopped;
            }
        }
        return true;
    }

    template< class XProcessorType >
    void KTBundleQueueProcessorTemplate< XProcessorType >::ClearQueue()
    {
        while (! fQueue.empty())
        {
            boost::shared_ptr<KTBundle> bundleToDelete;
            fQueue.wait_and_pop(bundleToDelete);
        }
        return;
    }


    template< class XProcessorType >
    void KTBundleQueueProcessorTemplate< XProcessorType >::QueueBundle(boost::shared_ptr<KTBundle> bundle)
    {
        KTDEBUG(eqplog, "Queueing bundle");
        fQueue.push(bundle);
        return;
    }

    template< class XProcessorType >
    void KTBundleQueueProcessorTemplate< XProcessorType >::QueueBundles(std::list< boost::shared_ptr<KTBundle> >* bundleList)
    {
        typedef std::list< boost::shared_ptr<KTBundle> > BundleList;

        KTDEBUG(eqplog, "Queueing bundles");
        while (! bundleList->empty())
        {
            fQueue.push(bundleList->front());
            bundleList->pop_front();
        }
        delete bundleList;
        return;
    }


} /* namespace Katydid */
#endif /* KTBUNDLEQUEUEPROCESSOR_HH_ */
