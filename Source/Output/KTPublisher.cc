/*
 * KTPublisher.cc
 *
 *  Created on: Sep 19, 2012
 *      Author: nsoblath
 */

#include "KTPublisher.hh"

#include "KTEvent.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    KTPublisher::KTPublisher() :
            KTConfigurable(),
            KTProcessor(),
            KTFactory< KTWriter >()
    {
        fConfigName = "writer";

        this->RegisterSlot("publish-event", this, &KTPublisher::Publish, "void (const KTEvent*)");
    }

    KTPublisher::~KTPublisher()
    {
        ClearPublicationList();
    }

    Bool_t KTPublisher::Configure(const KTPStoreNode* node)
    {
        return true;
    }

    Bool_t KTPublisher::AddDataToPublicationList(const string& writerName, const string& dataName)
    {
        PubMapIter pmIter = fPubMap.find(writerName);

        if (pmIter == fPubMap.end())
        {
            // Writer with writerName does not yet exist
            // Create a new writer
            KTWriter* newWriter = Create(writerName);
            if (newWriter == NULL)
            {
                // Failed to create the writer
                KTERROR(publog, "Failed to create new writer <" << writerName << "> while trying to add publication of data <" << dataName << ">");
                return false;
            }

            // Insert a new writer and data list into the publication map
            std::pair< PubMapIter, Bool_t > newInsert = fPubMap.insert(PubMapValue(writerName, WriterAndDataList));
            if (! newInsert.second)
            {
                KTERROR(publog, "Failed to insert new data list and writer into the publication map (writer = " << writerName << ";  data = " << dataName << ")");
                delete newWriter;
                return false;
            }
            // Insertion was successful
            pmIter = newInsert.first;
            // Hand off newWriter to the publication map
            pmIter->second.fWriter = newWriter;
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

        if (pmIter->second.fDataList.empty())
        {
            // Data list is now empty, so remove the writer from the publication list
            delete pmIter->second.fWriter;
            fPubMap.erase(pmIter);
        }

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


    void KTPublisher::Publish(const KTEvent* event)
    {
        // Loop over all writers in the publication map
        for (PubMapCIter pmIter = fPubMap.begin(); pmIter != fPubMap.end(); pmIter++)
        {
            KTWriter* writer = pmIter->second.fWriter;
            // Loop over all of the data types that have been requested for publication for this writer
            for (DataListIter dlIter = pmIter->second.fDataList.begin(); dlIter != pmIter->second.fDataList.end(); dlIter++)
            {
                const string dataName = *dlIter;
                // Attempt to get data from the event with this name
                KTWriteableData* data = event->GetData< KTWriteableData >(dataName);
                if (data != NULL)
                {
                    // Data exists and is writeable, to publish it!
                    writer->Publish(data);
                }
            }
        }
        return;
    }


} /* namespace Katydid */
