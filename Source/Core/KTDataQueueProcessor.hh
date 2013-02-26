/*
 * KTDataQueueProcessorTemplate.hh
 *
 *  Created on: Jan 10, 2013
 *      Author: nsoblath
 */

#ifndef KTDATAQUEUEPROCESSOR_HH_
#define KTDATAQUEUEPROCESSOR_HH_

#include "KTPrimaryProcessor.hh"

#include "KTConcurrentQueue.hh"
#include "KTData.hh"
#include "KTLogger.hh"

#include <boost/shared_ptr.hpp>

namespace Katydid
{
    KTLOGGER(eqplog, "katydid.core");

    //***********************************
    // Data Queue Processor Template
    //***********************************

    template< class XProcessorType >
    class KTDataQueueProcessorTemplate : public KTPrimaryProcessor
    {
        public:
            struct DataAndFunc
            {
                boost::shared_ptr< KTData > fData;
                void (XProcessorType::*fFuncPtr)(boost::shared_ptr<KTData>);
            };

            typedef KTConcurrentQueue< DataAndFunc > Queue;

            enum Status
            {
                kStopped,
                kRunning
            };

        public:
            KTDataQueueProcessorTemplate(const std::string& name = "default-data-queue-proc-template-name");
            virtual ~KTDataQueueProcessorTemplate();

            Status GetStatus() const;
            void SetStatus(KTDataQueueProcessorTemplate< XProcessorType >::Status);

        protected:
            Status fStatus;

            //**************************************
            // Derived Processor function pointer
            //**************************************
        public:
            void SetFuncPtr(void (XProcessorType::*ptr)(boost::shared_ptr<KTData>));

        protected:
            void (XProcessorType::*fFuncPtr)(boost::shared_ptr<KTData>);


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
            // Queueing functions for slots
            //*********
        protected:
            /// Queue an data object
            /// Assumes ownership of the data; original shared pointer will be nullified
            void DoQueueData(boost::shared_ptr<KTData>& data, void (XProcessorType::*func)(boost::shared_ptr<KTData>));

            /// Queue a list of data objects
            /// Assumes ownership of all data objects and the list; original shared pointers will be nullified
            //void DoQueueDataList(std::list< boost::shared_ptr<KTData>& >* dataList, void (XProcessorType::*fFuncPtr)(boost::shared_ptr<KTData>));
    };


    //**************************
    // Data Queue Processor
    //**************************

    class KTDataQueueProcessor : public KTDataQueueProcessorTemplate< KTDataQueueProcessor >
    {
        protected:
            typedef KTSignalConcept< void (boost::shared_ptr<KTData>) >::signal DataSignal;

        public:
            KTDataQueueProcessor(const std::string& name = "default-data-queue-proc-name");
            virtual ~KTDataQueueProcessor();

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
        public:
            /// Queue an data object; will emit data signal
            /// Assumes ownership of the data; original shared pointer will be nullified
            void QueueData(boost::shared_ptr<KTData>& data);

            /// Queue a list of data objects; will emit data signal
            /// Assumes ownership of all data objects and the list; original shared pointers will be nullified
            //void QueueDataList(std::list< boost::shared_ptr<KTData> >* dataList);

    };


    //**************************************************
    // Data Queue Processor Template Implementation
    //**************************************************


    template< class XProcessorType >
    KTDataQueueProcessorTemplate< XProcessorType >::KTDataQueueProcessorTemplate(const std::string& name) :
            KTPrimaryProcessor(name),
            fStatus(kStopped),
            fFuncPtr(NULL),
            fQueue()
    {
    }

    template< class XProcessorType >
    KTDataQueueProcessorTemplate< XProcessorType >::~KTDataQueueProcessorTemplate()
    {
        ClearQueue();
    }

    template< class XProcessorType >
    Bool_t KTDataQueueProcessorTemplate< XProcessorType >::Run()
    {
        fStatus = kRunning;
        return ProcessQueue();
    }

    template< class XProcessorType >
    void KTDataQueueProcessorTemplate< XProcessorType >::Stop()
    {
        fStatus = kStopped;
        fQueue.interrupt();
        return;
    }

    template< class XProcessorType >
    void KTDataQueueProcessorTemplate< XProcessorType >::SetFuncPtr(void (XProcessorType::*ptr)(boost::shared_ptr<KTData>))
    {
        fFuncPtr = ptr;
        return;
    }


    template< class XProcessorType >
    Bool_t KTDataQueueProcessorTemplate< XProcessorType >::ProcessQueue()
    {
        KTDEBUG(eqplog, "Beginning to process publication queue");
        while (fStatus != kStopped)
        {
            KTDEBUG(eqplog, "processing . . .");
            DataAndFunc daf;
            if (fQueue.wait_and_pop(daf))
            {
                KTDEBUG(eqplog, "Data acquired for publishing");
                (static_cast<XProcessorType*>(this)->*(daf.fFuncPtr))(daf.fData);
                if (daf.fData->fLastData) fStatus = kStopped;
            }
        }
        return true;
    }

    template< class XProcessorType >
    void KTDataQueueProcessorTemplate< XProcessorType >::ClearQueue()
    {
        while (! fQueue.empty())
        {
            DataAndFunc daf;
            fQueue.wait_and_pop(daf);
        }
        return;
    }


    template< class XProcessorType >
    void KTDataQueueProcessorTemplate< XProcessorType >::DoQueueData(boost::shared_ptr<KTData>& data, void (XProcessorType::*func)(boost::shared_ptr<KTData>))
    {
        KTDEBUG(eqplog, "Queueing data");
        DataAndFunc daf;
        daf.fData = data; // I'd like to use move semantics here (operator=(shared_ptr&&)), but they didn't work, so I bootstrapped with copy and reset.
        data.reset();
        daf.fFuncPtr = func;
        fQueue.push(daf);
        return;
    }
/*
    template< class XProcessorType >
    void KTDataQueueProcessorTemplate< XProcessorType >::DoQueueDataList(std::list< boost::shared_ptr<KTData>& >* dataList, void (XProcessorType::*func)(boost::shared_ptr<KTData>))
    {
        typedef std::list< boost::shared_ptr<KTData> > DataList;

        KTDEBUG(eqplog, "Queueing data objects");
        DataAndFunc daf;
        while (! dataList->empty())
        {
            daf.fData = &(dataList->front()); // using move semantics
            daf.fFuncPtr = func;
            dataList->pop_front();
            fQueue.push(daf);
        }
        delete dataList;
        return;
    }
*/

} /* namespace Katydid */
#endif /* KTDATAQUEUEPROCESSOR_HH_ */
