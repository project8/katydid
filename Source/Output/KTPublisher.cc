/*
 * KTPublisher.cc
 *
 *  Created on: Sep 19, 2012
 *      Author: nsoblath
 */

#include "KTPublisher.hh"

#include "KTBundle.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTWriteableData.hh"

using std::string;
using boost::shared_ptr;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedRegistrar< KTProcessor, KTPublisher > sPublisherProcRegistrar("publisher");

    KTPublisher::KTPublisher() :
            KTPrimaryProcessor(),
            fStatus(kStopped),
            fPubFactory(KTFactory< KTWriter >::GetInstance()),
            fPubMap(),
            fPubQueue()
    {
        fConfigName = "publisher";

        this->RegisterSlot("publish-bundle", this, &KTPublisher::Publish, "void (const KTBundle*)");
        this->RegisterSlot("queue-bundle", this, &KTPublisher::Queue, "void (KTBundle*)");
    }

    KTPublisher::~KTPublisher()
    {
        ClearPublicationQueue();
        ClearPublicationList();
    }

    Bool_t KTPublisher::Configure(const KTPStoreNode* node)
    {
        // Configure writers
        KTPStoreNode::csi_pair itPair = node->EqualRange("writer");
        for (KTPStoreNode::const_sorted_iterator it = itPair.first; it != itPair.second; it++)
        {
            KTPStoreNode subNode = KTPStoreNode(&(it->second));
            if (! subNode.HasData("type"))
            {
                KTERROR(publog, "Unable to add writer: no writer type given");
                return false;
            }
            string writerType = subNode.GetData("type");
            string writerName = subNode.GetData("name", writerType);
            KTWriter* newWriter = AddWriter(writerType, writerName);

            if (newWriter == NULL)
            {
                KTERROR(publog, "Something went wrong while creating the writer.");
                return false;
            }

            const KTPStoreNode* writerConfigNode = subNode.GetChild("configuration");
            if (! newWriter->Configure(writerConfigNode))
            {
                KTERROR(publog, "An error occurred while configuring writer <" << writerName << "> (a.k.a. " << writerType << ")");
                return false;
            }
        }

        // Add data to the publication list
        itPair = node->EqualRange("publish-data");
        for (KTPStoreNode::const_sorted_iterator it = itPair.first; it != itPair.second; it++)
        {
            KTPStoreNode subNode = KTPStoreNode(&(it->second));
            if (! subNode.HasData("writer-name"))
            {
                KTERROR(publog, "Unable to add data for publication; no writer name was given.");
                return false;
            }
            if (! subNode.HasData("data-name"))
            {
                KTERROR(publog, "Unable to add data for publication; no data name was given.");
                return false;
            }
            string writerName = subNode.GetData("writer-name");
            string dataName = subNode.GetData("data-name");

            if (! AddDataToPublicationList(writerName, dataName))
            {
                KTERROR(publog, "Something went wrong while adding data for publication: writer name: " << writerName << "; data name: " << dataName);
                return false;
            }
        }

        return true;
    }

    Bool_t KTPublisher::Run()
    {
        fStatus = kRunning;
        return ProcessQueue();
    }

    void KTPublisher::Stop()
    {
        fStatus = kStopped;
        fPubQueue.interrupt();
        return;
    }

    KTWriter* KTPublisher::AddWriter(const std::string& writerType, const std::string& writerName)
    {
        PubMapIter pmIter = fPubMap.find(writerName);

        if (pmIter == fPubMap.end())
        {
            // Writer with writerName does not yet exist
            // Create a new writer
            KTWriter* newWriter = fPubFactory->Create(writerType);
            if (newWriter == NULL)
            {
                // Failed to create the writer
                KTERROR(publog, "Failed to create new writer <" << writerName << "> of type <" << writerType << ">");
                return NULL;
            }

            // Insert a new writer and data list into the publication map
            std::pair< PubMapIter, Bool_t > newInsert = fPubMap.insert(PubMapValue(writerName, WriterAndDataList()));
            if (! newInsert.second)
            {
                KTERROR(publog, "Failed to insert new writer into the publication map; writer name: " << writerName << "; writer type: " << writerType);
                delete newWriter;
                return NULL;
            }
            // Insertion was successful
            pmIter = newInsert.first;
            // Hand off newWriter to the publication map
            pmIter->second.fWriter = newWriter;

            return newWriter;
        }

        KTWARN(publog, "A writer with name " << writerName << " has already been added; there is probably a mistake in the configuration file.");
        return NULL;
    }

    void KTPublisher::RemoveWriter(const std::string& writerName)
    {
        PubMapIter pmIter = fPubMap.find(writerName);
        if (pmIter == fPubMap.end())
        {
            // Writer with writerName was not found in the publication list
            return;
        }

        delete pmIter->second.fWriter;
        fPubMap.erase(pmIter);

        return;
    }

    Bool_t KTPublisher::AddDataToPublicationList(const string& writerName, const string& dataName)
    {
        PubMapIter pmIter = fPubMap.find(writerName);

        if (pmIter == fPubMap.end())
        {
            KTERROR(publog, "Did not find writer <" << writerName << ">. Please create and configure the writer in your configuration file.");
            return false;
        }

        // Add dataName to the data list
        pmIter->second.fDataList.insert(dataName);

        return true;
    }

    void KTPublisher::RemoveDataFromPublicationList(const string& writerName, const string& dataName)
    {
        PubMapIter pmIter = fPubMap.find(writerName);
        if (pmIter == fPubMap.end())
        {
            // Writer with writerName was not found in the publication list
            return;
        }

        // Writer with writerName was found in the publication list
        DataListIter dlIter = pmIter->second.fDataList.find(dataName);
        if (dlIter == pmIter->second.fDataList.end())
        {
            // Data with dataName was not found in the data list
            return;
        }

        // Data with dataName was found in the data list
        pmIter->second.fDataList.erase(dlIter);

        return;
    }

    void KTPublisher::ClearPublicationList()
    {
        for (PubMapIter pmIter = fPubMap.begin(); pmIter != fPubMap.end(); pmIter++)
        {
            delete pmIter->second.fWriter;
        }
        fPubMap.clear();
        return;
    }

    Bool_t KTPublisher::ProcessQueue()
    {
        KTDEBUG(publog, "Beginning to process publication queue");
        while (fStatus != kStopped)
        {
            KTDEBUG(publog, "processing . . .");
            shared_ptr<KTBundle> bundleToPublish;
            if (fPubQueue.wait_and_pop(bundleToPublish))
            {
                KTDEBUG(publog, "Bundle acquired for publishing");
                Publish(bundleToPublish);
                if (bundleToPublish->GetIsLastBundle()) fStatus = kStopped;
            }
        }
        return true;
    }

    void KTPublisher::ClearPublicationQueue()
    {
        while (! fPubQueue.empty())
        {
            shared_ptr<KTBundle> bundleToDelete;
            fPubQueue.wait_and_pop(bundleToDelete);
        }
        return;
    }


    void KTPublisher::Publish(shared_ptr<KTBundle> bundle)
    {
        if (fStatus == kStopped) return;

        // Loop over all writers in the publication map
        for (PubMapCIter pmIter = fPubMap.begin(); pmIter != fPubMap.end(); pmIter++)
        {
            KTWriter* writer = pmIter->second.fWriter;
            // Loop over all of the data types that have been requested for publication for this writer
            for (DataListIter dlIter = pmIter->second.fDataList.begin(); dlIter != pmIter->second.fDataList.end(); dlIter++)
            {
                const string dataName = *dlIter;
                // Attempt to get data from the bundle with this name
                KTWriteableData* data = bundle->GetData< KTWriteableData >(dataName);
                if (data != NULL)
                {
                    // Data exists and is writeable --- publish it!
                    writer->Publish(data);
                }
            }
        }
        return;
    }

    void KTPublisher::Queue(shared_ptr<KTBundle> bundle)
    {
        KTDEBUG(publog, "Queueing bundle");
        fPubQueue.push(bundle);
        return;
    }


} /* namespace Katydid */
