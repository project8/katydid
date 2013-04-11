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
            KTERROR(inlog, "A problem occured while parsing the mc-truth-events file");
            return shared_ptr<KTData>();
        }

        const rapidjson::Value& events = document["events"];
        if (! events.IsArray())
        {
            KTERROR(inlog, "\"events\" value in the mc truth file is either missing or not an array");
            return shared_ptr<KTData>();
        }

        for (rapidjson::Value::ConstValueIterator evIt = events.Begin(); evIt != events.End(); evIt++)
        {
            const rapidjson::Value& support = (*evIt)["support"];
            if (support.IsArray())
            {
                Double_t start = (*evIt)["support"][rapidjson::SizeType(0)].GetDouble(); // explicit cast to SizeType used because of abiguous overload
                Double_t end = (*evIt)["support"][rapidjson::SizeType(1)].GetDouble(); // explicit cast to SizeType used because of abiguous overload
                KTDEBUG(inlog, "extracted (" << start << ", " << end << ")");
            }
            else
            {
                KTWARN(inlog, "\"support\" value is either missing or not an array");
            }
        }

        return shared_ptr<KTData>();
    }

    shared_ptr< KTData > KTJSONReader::ReadAnalysisCandidatesFile()
    {
        return shared_ptr<KTData>();
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
