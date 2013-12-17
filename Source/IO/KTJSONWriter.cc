/*
 * KTJSONWriter.cc
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#include "KTJSONWriter.hh"

#include "KTNOFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedNORegistrar< KTWriter, KTJSONWriter > sJSONWriterRegistrar("json-writer");
    static KTDerivedNORegistrar< KTProcessor, KTJSONWriter > sJSONWProcRegistrar("json-writer");

    KTJSONWriter::KTJSONWriter(const std::string& name) :
            KTWriterWithTypists< KTJSONWriter >(name),
            fFilename("basic_output.json"),
            fFileMode("w+"),
            fPrettyJSONFlag(true),
            fFile(NULL),
            fFileStream(NULL),
            fJSONMaker(NULL)
    {
    }

    KTJSONWriter::~KTJSONWriter()
    {
        CloseFile();
    }

    bool KTJSONWriter::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetData<string>("output-file", fFilename));
            SetFileMode(node->GetData<string>("file-mode", fFileMode));
            SetPrettyJSONFlag(node->GetData<bool>("pretty-json", fPrettyJSONFlag));
        }

        return true;
    }

    bool KTJSONWriter::OpenFile()
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

    void KTJSONWriter::CloseFile()
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

    bool KTJSONWriter::OpenAndVerifyFile()
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
