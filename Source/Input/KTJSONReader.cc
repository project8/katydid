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

#include "document.h"
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
            //fFile(NULL),
            //fFileStream(NULL),
            //fJSONMaker(NULL),
            fMCTruthEventsSignal("mc-truth-events", this),
            fAnalysisCandidatesSignal("analysis-candidates", this)
    {
    }

    KTJSONReader::~KTJSONReader()
    {
        //CloseFile();
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
        switch (type)
        {
            case "mc-truth-electrons":
                fRunFcn = &KTJSONReader::ReadMCTruthEventsFile;
                break;
            case "analysis-candidates":
                fRunFcn = &KTJSONReader::ReadAnalysisCandidatesFile;
                break;
            default:
                KTERROR(inlog, "Invalid file type: " << fFileType);
                return false;
        }

        fFileType = type;

        return true;
    }

/*
    Bool_t KTJSONReader::OpenFile()
    {
        CloseFile();

        if (fFilename == "stdout")
        {
            fFileStream = new rapidjson::FileStream(stdout);
        }
        else
        {
            fFile = fopen(fFilename.c_str(), fFileMode.c_str());
            if (fFile == NULL)
            {
                KTERROR(publog, "File did not open\n" <<
                        "\tFilename: " << fFilename <<
                        "\tMode: " << fFileMode);
                return false;
            }
            fFileStream = new rapidjson::FileStream(fFile);
        }

        if (fPrettyJSONFlag)
        {
            fJSONMaker = new KTJSONMakerPretty< rapidjson::FileStream >(*fFileStream);
        }
        else
        {
            fJSONMaker = new KTJSONMakerCompact< rapidjson::FileStream >(*fFileStream);
        }

        fJSONMaker->StartObject();

        return true;
    }

    void KTJSONReader::CloseFile()
    {
        if (fJSONMaker != NULL)
        {
            fJSONMaker->EndObject();
            delete fJSONMaker;
            fJSONMaker = NULL;
        }
        if (fFileStream != NULL)
        {
            delete fFileStream;
            fFileStream = NULL;
        }
        if (fFile != NULL)
        {
            fclose(fFile);
            fFile = NULL;
        }
        return;
    }

    Bool_t KTJSONReader::OpenAndVerifyFile()
    {
        if (fFileStream == NULL || fJSONMaker == NULL)
        {
            if (! OpenFile())
            {
                return false;
            }
        }
        return true;
    }
*/
    shared_ptr< KTData > KTJSONReader::ReadMCTruthEventsFile()
    {
        FILE* file = fopen(fFilename.c_str(), fFileMode.c_str());
        if (file == NULL)
        {
            KTERROR(inlog, "File did not open\n" <<
                    "\tFilename: " << fFilename <<
                    "\tMode: " << fFileMode);
            return shared_ptr<KTData>();
        }
        rapidjson::FileStream fileStream = new rapidjson::FileStream(file);

        rapidjson::Document document;
        if (document.ParseStream<0>(fileStream).HasParseError())
        {
            KTERROR(inlog, "Unable to parse file <" << fFilename << ">");
            return shared_ptr<KTData>();
        }

        KTDEBUG(inlog, "Input file open and parsed: <" << fFilename << ">");

        rapidjson::GenericValue& events = document["events"];
        if (! events.IsArray())
        {
            KTERROR(inlog, "\"events\" value in the mc truth file is either missing or not an array");
            return shared_ptr<KTData>();
        }

        for (rapidjson::GenericValue::ConstValueIterator evIt = events.Begin(); evIt != events.End(); evIt++)
        {
            if (evIt->IsObject() && ! (*evIt)["support"].IsArray())
            {
                Double_t start = (*evIt)["support"][0].GetDouble();
                Double_t end = (*evIt)["support"][1].GetDoulbe();
                KTDEBUG(inlog, "extracted (" << start << ", " << end << ")");
            }
            else
            {
                KTWARN(inlog, "Invalid element in events array");
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
