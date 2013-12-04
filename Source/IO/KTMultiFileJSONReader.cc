/*
 * KTMultiFileJSONReader.cc
 *
 *  Created on: May 31, 2013
 *      Author: nsoblath
 */

#include "KTMultiFileJSONReader.hh"

#include "KTAnalysisCandidates.hh"
#include "KTCCResults.hh"
#include "KTFilenameParsers.hh"
#include "KTMCTruthEvents.hh"
#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

#include "filestream.h"



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
            fFileIter(fFilenames.end()),
            fFileMode("r"),
            fDataTypes(),
            fMCTruthEventsSignal("mc-truth-events", this),
            fAnalysisCandidatesSignal("analysis-candidates", this),
            fCCResultsSignal("cc-results", this),
            fDoneSignal("done", this),
            fAppendMCTruthEventsSlot("mc-truth-events", this, &KTMultiFileJSONReader::Append, &fMCTruthEventsSignal),
            fAppendAnalysisCandidatesSlot("analysis-candidates", this, &KTMultiFileJSONReader::Append, &fAnalysisCandidatesSignal),
            fAppendCCResultsSlot("cc-results", this, &KTMultiFileJSONReader::Append, &fCCResultsSignal)
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

        itPair = node->EqualRange("data-type");
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
        else if (type == "mc-truth-events")
        {
            fDataTypes.push_back(DataType(type, &KTMultiFileJSONReader::AppendMCTruthEvents, &fMCTruthEventsSignal));
        }
        else if (type == "analysis-candidates")
        {
            fDataTypes.push_back(DataType(type, &KTMultiFileJSONReader::AppendAnalysisCandidates, &fAnalysisCandidatesSignal));
        }
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

        KTINFO(inlog, "Input file open and parsed: <" << filename << ">");

        return true;
    }

    Bool_t KTMultiFileJSONReader::Run()
    {
        for (fFileIter = fFilenames.begin(); fFileIter != fFilenames.end(); fFileIter++)
        {
            rapidjson::Document document;
            if (! OpenAndParseFile(*fFileIter, document))
            {
                KTERROR(inlog, "A problem occurred while parsing file <" << *fFileIter << ">");
                return false;
            }

            KTDataPtr newData(new KTData());
            for (deque< DataType >::const_iterator dtIt = fDataTypes.begin(); dtIt != fDataTypes.end(); dtIt++)
            {
                KTDEBUG(inlog, "Appending data of type " << dtIt->fName);
                if (! (this->*(dtIt->fAppendFcn))(document, *(newData.get())))
                {
                    KTERROR(inlog, "Something went wrong while appending data of type <" << dtIt->fName << "> from <" << *fFileIter << ">");
                }
                (*(dtIt->fSignal))(newData);
            }
        }

        fDoneSignal();

        return true;
    }

    Bool_t KTMultiFileJSONReader::Append(KTData& data)
    {
        if (fFileIter == fFilenames.end())
        {
            KTERROR(inlog, "File iterator has already reached the end of the filenames");
            return false;
        }

        rapidjson::Document document;
        if (! OpenAndParseFile(*fFileIter, document))
        {
            KTERROR(inlog, "A problem occurred while parsing file <" << *fFileIter << ">");
            return false;
        }

        for (deque< DataType >::const_iterator dtIt = fDataTypes.begin(); dtIt != fDataTypes.end(); dtIt++)
        {
            KTDEBUG(inlog, "Appending data of type " << dtIt->fName);
            if (! (this->*(dtIt->fAppendFcn))(document, data))
            {
                KTERROR(inlog, "Something went wrong while appending data of type <" << dtIt->fName << "> from <" << *fFileIter << ">");
            }
        }

        fFileIter++;

        return true;
    }

    Bool_t KTMultiFileJSONReader::AppendMCTruthEvents(rapidjson::Document& document, KTData& appendToData)
    {
        if (! document["record_size"].IsUint())
        {
            KTERROR(inlog, "\"record_size\" value is missing or is not an unsigned integer");
            return false;
        }
        UInt_t recordSize = document["record_size"].GetUint();

        if (! document["records_simulated"].IsUint())
        {
            KTERROR(inlog, "\"records_simulated\" value is missing or is not an unsigned integer");
            return false;
        }
        UInt_t recordsSimulated = document["records_simulated"].GetUint();

        if (! document["egg_name"].IsString())
        {
            KTERROR(inlog, "\"egg_name\" value is missing or is not a string");
            return false;
        }
        KTLocustMCFilename parsedFilename(document["egg_name"].GetString());

        const rapidjson::Value& events = document["events"];
        if (! events.IsArray())
        {
            KTERROR(inlog, "\"events\" value in the mc truth file is either missing or not an array");
            return false;
        }

        KTMCTruthEvents& mcTruth = appendToData.Of< KTMCTruthEvents >();
        mcTruth.SetEventLength(parsedFilename.fEventLength);
        mcTruth.Setdfdt(parsedFilename.fdfdt);
        mcTruth.SetSignalPower(parsedFilename.fSignalPower);

        mcTruth.SetRecordSize(recordSize);
        mcTruth.SetNRecords(recordsSimulated);

        for (rapidjson::Value::ConstValueIterator evIt = events.Begin(); evIt != events.End(); evIt++)
        {
            const rapidjson::Value& support = (*evIt)["support"];
            if (support.IsArray())
            {
                UInt_t startRec = support[rapidjson::SizeType(0)].GetUint(); // explicit cast of array index to SizeType used because of abiguous overload
                UInt_t startSample = support[rapidjson::SizeType(1)].GetUint(); // explicit cast of array index to SizeType used because of abiguous overload
                UInt_t endRec = support[rapidjson::SizeType(2)].GetUint(); // explicit cast of array index to SizeType used because of abiguous overload
                UInt_t endSample = support[rapidjson::SizeType(3)].GetUint(); // explicit cast of array index to SizeType used because of abiguous overload
                KTDEBUG(inlog, "extracted (" << startRec << ", " << startSample << ", " << endRec << ", " << endSample << ")");
                if (endRec < startRec || (endRec == startRec && endSample < startSample))
                {
                    KTWARN(inlog, "Invalid event: (" << startRec << ", " << startSample << " --> " << endRec << ", " << endSample << ")");
                }
                else
                {
                    mcTruth.AddEvent(KTMCTruthEvents::Event(startRec, startSample, endRec, endSample));
                }
            }
            else
            {
                KTWARN(inlog, "\"support\" value is either missing or not an array");
            }
        }

        KTDEBUG(inlog, "new data object has " << mcTruth.GetEvents().size() << " events");

        return true;
    }

    Bool_t KTMultiFileJSONReader::AppendAnalysisCandidates(rapidjson::Document& document, KTData& appendToData)
    {
        if (! document["record_size"].IsUint())
        {
            KTERROR(inlog, "\"record_size\" value is missing or is not an unsigned integer");
            return false;
        }
        UInt_t recordSize = document["record_size"].GetUint();

        if (! document["records_analyzed"].IsUint())
        {
            KTERROR(inlog, "\"records_analyzed\" value is missing or is not an unsigned integer");
            return false;
        }
        UInt_t recordsAnalyzed = document["records_analyzed"].GetUint();

        const rapidjson::Value& events = document["candidates"];
        if (! events.IsArray())
        {
            KTERROR(inlog, "\"candidates\" value in the analysis candidates file is either missing or not an array");
            return false;
        }

        KTAnalysisCandidates& candidates = appendToData.Of< KTAnalysisCandidates >();
        candidates.SetRecordSize(recordSize);
        candidates.SetNRecords(recordsAnalyzed);

        for (rapidjson::Value::ConstValueIterator evIt = events.Begin(); evIt != events.End(); evIt++)
        {
            const rapidjson::Value& support = (*evIt)["support"];
            if (support.IsArray())
            {
                UInt_t startRec = support[rapidjson::SizeType(0)].GetUint(); // explicit cast of array index to SizeType used because of abiguous overload
                UInt_t startSample = support[rapidjson::SizeType(1)].GetUint(); // explicit cast of array index to SizeType used because of abiguous overload
                UInt_t endRec = support[rapidjson::SizeType(2)].GetUint(); // explicit cast of array index to SizeType used because of abiguous overload
                UInt_t endSample = support[rapidjson::SizeType(3)].GetUint(); // explicit cast of array index to SizeType used because of abiguous overload
                KTDEBUG(inlog, "extracted (" << startRec << ", " << startSample << ", " << endRec << ", " << endSample << ")");
                if (endRec < startRec || (endRec == startRec && endSample < startSample))
                {
                    KTWARN(inlog, "Invalid candidate: (" << startRec << ", " << startSample << " --> " << endRec << ", " << endSample << ")");
                }
                else
                {
                    candidates.AddCandidate(KTAnalysisCandidates::Candidate(startRec, startSample, endRec, endSample));
                }
            }
            else
            {
                KTWARN(inlog, "\"support\" value is either missing or not an array");
            }
        }

        KTDEBUG(inlog, "new data object has " << candidates.GetCandidates().size() << " candidates");

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
        ccResultsData.SetEventLength(ccResults["event-length"].GetDouble());
        ccResultsData.Setdfdt(ccResults["dfdt"].GetDouble());
        ccResultsData.SetSignalPower(ccResults["signal-power"].GetDouble());
        ccResultsData.SetNEvents(ccResults["n-events"].GetUint());
        ccResultsData.SetNCandidates(ccResults["n-candidates"].GetUint());

        const rapidjson::Value& newxcm = ccResults["n-events-with-x-cand-matches"];
        if (! newxcm.IsArray())
        {
            KTERROR(inlog, "\"n-events-with-x-cand-matches\" value is either missing or is not an array");
            return false;
        }

        UInt_t index = 0;
        ccResultsData.ResizeNEventsWithXCandidateMatches(newxcm.Size());
        for (rapidjson::Value::ConstValueIterator newxcmIt = newxcm.Begin(); newxcmIt != newxcm.End(); newxcmIt++)
        {
            //UInt_t thisNEvents = (*newxcmIt).GetUint();
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
        ccResultsData.ResizeNCandidatesWithXEventMatches(ncwxem.Size());
        for (rapidjson::Value::ConstValueIterator ncwxemIt = ncwxem.Begin(); ncwxemIt != ncwxem.End(); ncwxemIt++)
        {
            //UInt_t thisNEvents = (*ncwxemIt).GetUint();
            ccResultsData.SetNEventsWithXCandidateMatches(index, (*ncwxemIt).GetUint());
            index++;
        }

        ccResultsData.SetEfficiency(ccResults["efficiency"].GetDouble());
        ccResultsData.SetFalseRate(ccResults["false-rate"].GetDouble());

        return true;
    }

} /* namespace Katydid */
