/*
 * KTOneShotJSONReader.cc
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#include "KTOneShotJSONReader.hh"

#include "KTAnalysisCandidates.hh"
#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTMCTruthEvents.hh"
#include "KTPStoreNode.hh"

#include "filestream.h"

using boost::shared_ptr;
using std::string;

namespace Katydid
{
    KTLOGGER(inlog, "katydid.input");

    static KTDerivedRegistrar< KTReader, KTOneShotJSONReader > sJSONReaderRegistrar("oneshot-json-reader");
    static KTDerivedRegistrar< KTProcessor, KTOneShotJSONReader > sJSONRProcRegistrar("oneshot-json-reader");

    KTOneShotJSONReader::KTOneShotJSONReader(const std::string& name) :
            KTReader(name),
            fFilename("input.json"),
            fFileMode("r"),
            fFileType("mc-truth-events"),
            fRunFcn(&KTOneShotJSONReader::RunMCTruthEventsFile),
            fMCTruthEventsSignal("mc-truth-events", this),
            fAnalysisCandidatesSignal("analysis-candidates", this),
            fAppendMCTruthEventsSlot("append-mc-truth-events", this, &KTOneShotJSONReader::AppendMCTruthEventsFile, &fMCTruthEventsSignal),
            fAppendAnalysisCandidatesSlot("append-analysis-candidates", this, &KTOneShotJSONReader::AppendAnalysisCandidatesFile, &fAnalysisCandidatesSignal)
    {
    }

    KTOneShotJSONReader::~KTOneShotJSONReader()
    {
    }

    Bool_t KTOneShotJSONReader::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node == NULL) return false;

        SetFilename(node->GetData<string>("input-file", fFilename));
        SetFileMode(node->GetData<string>("file-mode", fFileMode));
        if (! SetFileType(node->GetData<string>("file-type", fFileType)))
        {
            return false;
        }

        return true;
    }

    Bool_t KTOneShotJSONReader::SetFileType(const std::string& type)
    {
        // set the read function pointer based on the file type
        if (type == "mc-truth-events")
        {
            fRunFcn = &KTOneShotJSONReader::RunMCTruthEventsFile;
        }
        else if (type == "analysis-candidates")
        {
            fRunFcn = &KTOneShotJSONReader::RunAnalysisCandidatesFile;
        }
        else
        {
            KTERROR(inlog, "Invalid file type: " << fFileType);
            return false;
        }

        fFileType = type;

        return true;
    }

    Bool_t KTOneShotJSONReader::OpenAndParseFile(rapidjson::Document& document)
    {
        FILE* file = fopen(fFilename.c_str(), fFileMode.c_str());
        if (file == NULL)
        {
            KTERROR(inlog, "File did not open\n" <<
                    "\tFilename: " << fFilename <<
                    "\tMode: " << fFileMode);
            return false;
        }

        rapidjson::FileStream fileStream(file);

        if (document.ParseStream<0>(fileStream).HasParseError())
        {
            KTERROR(inlog, "Unable to parse file <" << fFilename << ">\n" <<
                    "\tReason: " << document.GetParseError() << '\n' <<
                    "\tLocation: character (sorry!) " << document.GetErrorOffset());
            fclose(file);
            return false;
        }

        fclose(file);

        KTDEBUG(inlog, "Input file open and parsed: <" << fFilename << ">");

        return true;
    }

    Bool_t KTOneShotJSONReader::ReadMCTruthEventsFile(KTData& appendToData)
    {
        rapidjson::Document document;
        if (! OpenAndParseFile(document))
        {
            KTERROR(inlog, "A problem occurred while parsing the mc-truth-events file");
            return false;
        }

        if (! document["record_size"].IsUint())
        {
            KTERROR(inlog, "\"record_size\" value is missing or is not an unsigned integer");
            return false;
        }
        UInt_t recordSize = document["record_size"].GetUint();

        const rapidjson::Value& events = document["events"];
        if (! events.IsArray())
        {
            KTERROR(inlog, "\"events\" value in the mc truth file is either missing or not an array");
            return false;
        }

        KTMCTruthEvents& mcTruth = appendToData.Of< KTMCTruthEvents >();
        mcTruth.SetRecordSize(recordSize);

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

    Bool_t KTOneShotJSONReader::ReadAnalysisCandidatesFile(KTData& appendToData)
    {
        rapidjson::Document document;
        if (! OpenAndParseFile(document))
        {
            KTERROR(inlog, "A problem occurred while parsing the mc-truth-events file");
            return false;
        }

        if (! document["record_size"].IsUint())
        {
            KTERROR(inlog, "\"record_size\" value is missing or is not an unsigned integer");
            return false;
        }
        UInt_t recordSize = document["record_size"].GetUint();

         const rapidjson::Value& events = document["candidates"];
        if (! events.IsArray())
        {
            KTERROR(inlog, "\"candidates\" value in the analysis candidates file is either missing or not an array");
            return false;
        }

        KTAnalysisCandidates& candidates = appendToData.Of< KTAnalysisCandidates >();
        candidates.SetRecordSize(recordSize);

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

    Bool_t KTOneShotJSONReader::AppendMCTruthEventsFile(KTData& appendToData)
    {
        if (ReadMCTruthEventsFile(appendToData))
        {
            return true;
        }
        return false;
    }

    Bool_t KTOneShotJSONReader::AppendAnalysisCandidatesFile(KTData& appendToData)
    {
        if (ReadAnalysisCandidatesFile(appendToData))
        {
            return true;
        }
        return false;
    }

    Bool_t KTOneShotJSONReader::RunMCTruthEventsFile()
    {
        shared_ptr< KTData > newData(new KTData());
        if (! ReadMCTruthEventsFile(*(newData.get())))
        {
            KTERROR(inlog, "Something went wrong while reading the mc-truth-electrons file <" << fFilename << ">");
            return false;
        }
        fMCTruthEventsSignal(newData);
        return true;
    }

    Bool_t KTOneShotJSONReader::RunAnalysisCandidatesFile()
    {
        shared_ptr< KTData > newData(new KTData());
        if (! ReadAnalysisCandidatesFile(*(newData.get())))
        {
            KTERROR(inlog, "Something went wrong while reading the analysis-candidates file <" << fFilename << ">");
            return false;
        }
        fAnalysisCandidatesSignal(newData);
        return true;
    }


} /* namespace Katydid */
