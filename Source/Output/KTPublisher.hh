/*
 * KTPublisher.hh
 *
 *  Created on: Sep 19, 2012
 *      Author: nsoblath
 */

#ifndef KTPUBLISHER_HH_
#define KTPUBLISHER_HH_

#include "KTFactory.hh"
#include "KTPrimaryProcessor.hh"

#include "KTConcurrentQueue.hh"
#include "KTWriter.hh"

#include <set>

namespace Katydid
{
    class KTEvent;
    class KTPStoreNode;

    class KTPublisher : public KTPrimaryProcessor, public KTFactory< KTWriter >
    {
        protected:
            typedef std::set< std::string > DataList;
            typedef DataList::iterator DataListIter;
            typedef DataList::const_iterator DataListCIter;

            struct WriterAndDataList
            {
                KTWriter* fWriter;
                DataList fDataList;
            };

            typedef std::map< std::string, WriterAndDataList > PublicationMap;
            typedef PublicationMap::iterator PubMapIter;
            typedef PublicationMap::const_iterator PubMapCIter;
            typedef PublicationMap::value_type PubMapValue;

            typedef KTConcurrentQueue< KTEvent* > PublicationQueue;

            enum Status
            {
                kStopped,
                kRunning
            };

        public:
            KTPublisher();
            virtual ~KTPublisher();

        public:
            Bool_t Configure(const KTPStoreNode* node);

            Bool_t Run();

            void Stop();

            Status GetStatus() const;
            void SetStatus(KTPublisher::Status);

        protected:
            Status fStatus;


            //**************************
            // Publication map access
            //**************************
        public:
            Bool_t AddDataToPublicationList(const std::string& writerName, const std::string& dataName);
            void RemoveDataFromPublicationList(const std::string& writerName, const std::string& dataName);
            void ClearPublicationList();

        protected:
            PublicationMap fPubMap;


            //*********************
            // Publication queue
            //*********************
        public:
            Bool_t ProcessQueue();

            void ClearPublicationQueue();

        protected:
            PublicationQueue fPubQueue;


            //*********
            // Slots
            //*********
        public:
            void Publish(const KTEvent* event);
            /// Queue and event for publication
            /// Assumes ownership of the event
            /// If processor is not running, initiates the runnning of the processor (i.e. calls Run())
            void Queue(KTEvent* event);

    };

} /* namespace Katydid */
#endif /* KTPUBLISHER_HH_ */
