/*
 * KTMultiFileJSONReader.cc
 *
 *  Created on: May 31, 2013
 *      Author: nsoblath
 */

#include "KTMultiFileJSONReader.hh"

#include "KTCCResults.hh"
#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include "filestream.h"

using boost::shared_ptr;

using std::deque;
using std::string;

namespace Katydid
{
    KTLOGGER(inlog, "katydid.input");

    static KTDerivedNORegistrar< KTReader, KTMultiFileJSONReader > sMFJSONReaderRegistrar("multifile-json-reader");
    static KTDerivedNORegistrar< KTProcessor, KTMultiFileJSONReader > sMFJSONRProcRegistrar("multifile-json-reader");

    KTMultiFileJSONReader::KTMultiFileJSONReader(const std::string& name) :
            KTReader(name),
            fFilenames(),
            fFileMode("r"),
            fDataTypes(),
            fCCResultsSignal("cc-results", this)
            //fAppendCCResultsSlot("append-cc-results", this, &KTMultiFileJSONReader::AppendCCResults, &fCCResultsSignal)
    {
    }

    KTMultiFileJSONReader::~KTMultiFileJSONReader()
    {
    }

    Bool_t KTMultiFileJSONReader::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node == NULL) return false;

        KTPStoreNode::csi_pair itPair = node->EqualRange("input-file");
        for (KTPStoreNode::const_sorted_iterator it = itPair.first; it != itPair.second; it++)
        {
            AddFilename(it->second.get_value<string>());
            KTDEBUG(inlog, "Added filename <" << fFilenames.back() << ">");
        }

        SetFileMode(node->GetData<string>("file-mode", fFileMode));

        itPair = node->EqualRange("file-type");
        for (KTPStoreNode::const_sorted_iterator it = itPair.first; it != itPair.second; it++)
        {
            AddDataType(it->second.get_value<string>());
            KTDEBUG(inlog, "Added filename <" << fFilenames.back() << ">");
        }

        return true;
    }

    Bool_t KTMultiFileJSONReader::AddDataType(const std::string& type)
    {
        if (type == "cc-results")
        {
            fDataTypes.push_back(DataType(type, &KTMultiFileJSONReader::AppendCCResults, &fCCResultsSignal));
        }
        /*
        else if (type == "...")
        {
            fDataTypes.push_back(DataType(type, &KTMultiFileJSONReader::Append..., &f...Signal));
        }
        */
        else
        {
            KTERROR(inlog, "Invalid run-data-type: " << type);
            return false;
        }

        return true;
    }

    Bool_t KTMultiFileJSONReader::OpenAndParseFile(const string& filename, rapidjson::Document& document) const
    {
        FILE* file = fopen(filename.c_str(), fFileMode.c_str());
        if (file == NULL)
        {
            KTERROR(inlog, "File did not open\n" <<
                    "\tFilename: " << filename <<
                    "\tMode: " << fFileMode);
            return false;
        }

        rapidjson::FileStream fileStream(file);

        if (document.ParseStream<0>(fileStream).HasParseError())
        {
            KTERROR(inlog, "Unable to parse file <" << filename << ">\n" <<
                    "\tReason: " << document.GetParseError() << '\n' <<
                    "\tLocation: character (sorry!) " << document.GetErrorOffset());
            fclose(file);
            return false;
        }

        fclose(file);

        KTDEBUG(inlog, "Input file open and parsed: <" << filename << ">");

        return true;
    }

    inline Bool_t KTMultiFileJSONReader::Run()
    {
        for (deque< string >::const_iterator fileIt = fFilenames.begin(); fileIt != fFilenames.end(); fileIt++)
        {
            rapidjson::Document document;
            if (! OpenAndParseFile(*fileIt, document))
            {
                KTERROR(inlog, "A problem occurred while parsing file <" << *fileIt << ">");
                return false;
            }

            shared_ptr< KTData > newData(new KTData());
            for (deque< DataType >::const_iterator dtIt = fDataTypes.begin(); dtIt != fDataTypes.end(); dtIt++)
            {
                if (! (this->*(dtIt->fAppendFcn))(document, *(newData.get())))
                {
                    KTERROR(inlog, "Something went wrong while appending data of type <" << dtIt->fName << "> from <" << *fileIt << ">");
                }
                (*(dtIt->fSignal))(newData);
            }
        }
        return true;
    }

    Bool_t KTMultiFileJSONReader::AppendCCResults(rapidjson::Document& document, KTData& appendToData)
    {
        const rapidjson::Value& ccResults = document["cc-results"];
        if (! ccResults.IsObject())
        {
            KTERROR(inlog, "\"cc-results\" value is missing or is not an object");
            return false;
        }

        KTCCResults& ccResultsData = appendToData.Of< KTCCResults >();
        ccResultsData.SetNEvents(ccResults["n-events"].GetUint());
        ccResultsData.SetNCandidates(ccResults["n-candidates"].GetUint());

        const rapidjson::Value& newxcm = ccResults["n-events-with-x-cand-matches"];
        if (! newxcm.IsArray())
        {
            KTERROR(inlog, "\"n-events-with-x-cand-matches\" value is either missing or is not an array");
            return false;
        }

        UInt_t index = 0;
        for (rapidjson::Value::ConstValueIterator newxcmIt = newxcm.Begin(); newxcmIt != newxcm.End(); newxcmIt++)
        {
            UInt_t thisNEvents = (*newxcmIt).GetUint();
            ccResultsData.SetNEventsWithXCandidateMatches(index, (*newxcmIt).GetUint());
            index++;
        }

        const rapidjson::Value& ncwxem = ccResults["n-cands-with-x-event-matches"];
        if (! ncwxem.IsArray())
        {
            KTERROR(inlog, "\"n-events-with-x-cand-matches\" value is either missing or is not an array");
            return false;
        }

        index = 0;
        for (rapidjson::Value::ConstValueIterator ncwxemIt = ncwxem.Begin(); ncwxemIt != ncwxem.End(); ncwxemIt++)
        {
            UInt_t thisNEvents = (*ncwxemIt).GetUint();
            ccResultsData.SetNEventsWithXCandidateMatches(index, (*ncwxemIt).GetUint());
            index++;
        }

        ccResultsData.SetEfficiency(ccResults["efficiency"].GetDouble());
        ccResultsData.SetFalseRate(ccResults["false-rate"].GetDouble());

        return true;
    }

} /* namespace Katydid */
