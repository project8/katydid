/*
 * KTJSONReader.cc
 *
 *  Created on: Apr 11, 2013
 *      Author: nsoblath
 */

#include "KTJSONReader.hh"

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

    static KTDerivedRegistrar< KTReader, KTJSONReader > sJSONReaderRegistrar("json-reader");
    static KTDerivedRegistrar< KTProcessor, KTJSONReader > sJSONRProcRegistrar("json-reader");

    KTJSONReader::KTJSONReader(const std::string& name) :
            KTReader(name),
            fFilename("input.json"),
            fFileMode("r"),
            fFileType("mc-truth-events"),
            fRunFcn(&KTJSONReader::RunMCTruthEventsFile),
            fMCTruthEventsSignal("mc-truth-events", this),
            fAnalysisCandidatesSignal("analysis-candidates", this)
    {
    }

    KTJSONReader::~KTJSONReader()
    {
    }

    Bool_t KTJSONReader::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node == NULL) return false;

        SetFilename(node->GetData<string>("output-file", fFilename));
        SetFileMode(node->GetData<string>("file-mode", fFileMode));
        if (! SetFileType(node->GetData<string>("file-type", fFileType)))
        {
            return false;
        }

        return true;
    }

    Bool_t KTJSONReader::SetFileType(const std::string& type)
    {
        // set the read function pointer based on the file type
        if (type == "mc-truth-electrons")
        {
            fRunFcn = &KTJSONReader::RunMCTruthEventsFile;
        }
        else if (type == "analysis-candidates")
        {
            fRunFcn = &KTJSONReader::RunAnalysisCandidatesFile;
        }
        else
        {
            KTERROR(inlog, "Invalid file type: " << fFileType);
            return false;
        }

        fFileType = type;

        return true;
    }

    Bool_t KTJSONReader::OpenAndParseFile(rapidjson::Document& document)
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

    shared_ptr< KTData > KTJSONReader::ReadMCTruthEventsFile()
    {
        rapidjson::Document document;
        if (! OpenAndParseFile(document))
        {
            KTERROR(inlog, "A problem occurred while parsing the mc-truth-events file");
            return shared_ptr<KTData>();
        }

        if (! document["record_size"].IsUint())
        {
            KTERROR(inlog, "\"record_size\" value is missing or is not an unsigned integer");
            return shared_ptr<KTData>();
        }
        UInt_t recordSize = document["record_size"].GetUint();

        const rapidjson::Value& events = document["events"];
        if (! events.IsArray())
        {
            KTERROR(inlog, "\"events\" value in the mc truth file is either missing or not an array");
            return shared_ptr<KTData>();
        }

        boost::shared_ptr< KTData > newData(new KTData());
        KTMCTruthEvents& mcTruth = newData->Of< KTMCTruthEvents >();
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

        return newData;
    }

    shared_ptr< KTData > KTJSONReader::ReadAnalysisCandidatesFile()
    {
        rapidjson::Document document;
        if (! OpenAndParseFile(document))
        {
            KTERROR(inlog, "A problem occurred while parsing the mc-truth-events file");
            return shared_ptr<KTData>();
        }

        if (! document["record_size"].IsUint())
        {
            KTERROR(inlog, "\"record_size\" value is missing or is not an unsigned integer");
            return shared_ptr<KTData>();
        }
        UInt_t recordSize = document["record_size"].GetUint();

         const rapidjson::Value& events = document["candidates"];
        if (! events.IsArray())
        {
            KTERROR(inlog, "\"candidates\" value in the analysis candidates file is either missing or not an array");
            return shared_ptr<KTData>();
        }

        boost::shared_ptr< KTData > newData(new KTData());
        KTAnalysisCandidates& candidates = newData->Of< KTAnalysisCandidates >();
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

        KTDEBUG(inlog, "new data object has " << candidates.GetCandidates().size() << " events");

        return newData;
    }

    Bool_t KTJSONReader::RunMCTruthEventsFile()
    {
        shared_ptr< KTData > newData = ReadMCTruthEventsFile();
        if (! newData)
        {
            KTERROR(inlog, "Something went wrong while reading the mc-truth-electrons file <" << fFilename << ">");
            return false;
        }
        fMCTruthEventsSignal(newData);
        return true;
    }

    Bool_t KTJSONReader::RunAnalysisCandidatesFile()
    {
        shared_ptr< KTData > newData = ReadAnalysisCandidatesFile();
        if (! newData)
        {
            KTERROR(inlog, "Something went wrong while reading the analysis-candidates file <" << fFilename << ">");
            return false;
        }
        fAnalysisCandidatesSignal(newData);
        return true;
    }


} /* namespace Katydid */
