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
#include "KTData.hh"
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
            typedef KTConcurrentQueue< boost::shared_ptr<KTData> > Queue;

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
            void SetFuncPtr(void (XProcessorType::*ptr)(boost::shared_ptr<KTData>));

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
            /// Queue an data object
            /// Assumes ownership of the data
            void QueueData(boost::shared_ptr<KTData> data);

            /// Queue a list of data objects
            /// Assumes ownership of all data objects and the list
            void QueueDataList(std::list< boost::shared_ptr<KTData> >* dataList);
    };


    //**************************
    // Bundle Queue Processor
    //**************************

    class KTBundleQueueProcessor : public KTBundleQueueProcessorTemplate< KTBundleQueueProcessor >
    {
        protected:
            typedef KTSignal< void (boost::shared_ptr<KTData>) >::signal DataSignal;

        public:
            KTBundleQueueProcessor();
            virtual ~KTBundleQueueProcessor();

            Bool_t Configure(const KTPStoreNode* node);

        public:
            void EmitDataSignal(boost::shared_ptr<KTData> data);

            //***************
            // Signals
            //***************

        private:
            DataSignal fDataSignal;

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
            RegisterSlot("data", this, &KTBundleQueueProcessorTemplate< XProcessorType >::QueueData, "void (shared_ptr<KTData>)");
            RegisterSlot("data-list", this, &KTBundleQueueProcessorTemplate< XProcessorType >::QueueDataList, "void (list< shared_ptr<KTData> >)");
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
    void KTBundleQueueProcessorTemplate< XProcessorType >::SetFuncPtr(void (XProcessorType::*ptr)(boost::shared_ptr<KTData>))
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
            boost::shared_ptr<KTData> dataToPublish;
            if (fQueue.wait_and_pop(dataToPublish))
            {
                KTDEBUG(eqplog, "Bundle acquired for publishing");
                (static_cast<XProcessorType*>(this)->*fFuncPtr)(dataToPublish);
                if (dataToPublish->fLastData) fStatus = kStopped;
            }
        }
        return true;
    }

    template< class XProcessorType >
    void KTBundleQueueProcessorTemplate< XProcessorType >::ClearQueue()
    {
        while (! fQueue.empty())
        {
            boost::shared_ptr<KTData> dataToDelete;
            fQueue.wait_and_pop(dataToDelete);
        }
        return;
    }


    template< class XProcessorType >
    void KTBundleQueueProcessorTemplate< XProcessorType >::QueueData(boost::shared_ptr<KTData> data)
    {
        KTDEBUG(eqplog, "Queueing data");
        fQueue.push(data);
        return;
    }

    template< class XProcessorType >
    void KTBundleQueueProcessorTemplate< XProcessorType >::QueueDataList(std::list< boost::shared_ptr<KTData> >* dataList)
    {
        typedef std::list< boost::shared_ptr<KTBundle> > BundleList;

        KTDEBUG(eqplog, "Queueing bundles");
        while (! dataList->empty())
        {
            fQueue.push(dataList->front());
            dataList->pop_front();
        }
        delete dataList;
        return;
    }


} /* namespace Katydid */
#endif /* KTBUNDLEQUEUEPROCESSOR_HH_ */
