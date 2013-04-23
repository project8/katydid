/*
 * KTOfficialCandidatesWriter.cc
 *
 *  Created on: Apr 23, 2013
 *      Author: nsoblath
 */

#include "KTOfficialCandidatesWriter.hh"

#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedRegistrar< KTWriter, KTOfficialCandidatesWriter > sOCWriterRegistrar("official-candidates-writer");
    static KTDerivedRegistrar< KTProcessor, KTOfficialCandidatesWriter > sOCWProcRegistrar("official-candidates-writer");

    KTOfficialCandidatesWriter::KTOfficialCandidatesWriter(const std::string& name) :
            KTWriter(name),
            fFilename("basic_output.json"),
            fFileMode("w+"),
            fPrettyJSONFlag(true),
            fFile(NULL),
            fFileStream(NULL),
            fJSONMaker(NULL)
    {
    }

    KTOfficialCandidatesWriter::~KTOfficialCandidatesWriter()
    {
        CloseFile();
    }

    Bool_t KTOfficialCandidatesWriter::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetData<string>("output-file", fFilename));
            SetFileMode(node->GetData<string>("file-mode", fFileMode));
            SetPrettyJSONFlag(node->GetData<Bool_t>("pretty-json", fPrettyJSONFlag));
        }

        return true;
    }

    Bool_t KTOfficialCandidatesWriter::OpenFile()
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

    void KTOfficialCandidatesWriter::CloseFile()
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

    Bool_t KTOfficialCandidatesWriter::OpenAndVerifyFile()
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

} /* namespace Katydid */
