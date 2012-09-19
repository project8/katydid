/*
 * KTPublisher.hh
 *
 *  Created on: Sep 19, 2012
 *      Author: nsoblath
 */

#ifndef KTPUBLISHER_HH_
#define KTPUBLISHER_HH_

#include "KTConfigurable.hh"
#include "KTFactory.hh"
#include "KTProcessor.hh"

#include "KTWriter.hh"

#include <set>

namespace Katydid
{
    class KTEvent;
    class KTPStoreNode;

    class KTPublisher : public KTConfigurable, public KTProcessor, public KTFactory< KTWriter >
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

        public:
            KTPublisher();
            virtual ~KTPublisher();

        public:
            Bool_t Configure(const KTPStoreNode* node);


            //**************************
            // Publication map access
            //**************************
        public:
            Bool_t AddDataToPublicationList(const std::string& writerName, const std::string& dataName);
            void RemoveDataFromPublicationList(const std::string& writerName, const std::string& dataName);
            void ClearPublicationList();

        protected:
            PublicationMap fPubMap;


            //*********
            // Slots
            //*********
        public:
            void Publish(const KTEvent* event);

    };

} /* namespace Katydid */
#endif /* KTPUBLISHER_HH_ */
