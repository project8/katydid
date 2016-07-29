/*
 * KTJSONWriter.cc
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#include "KTJSONWriter.hh"

#include "KTParam.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTJSONWriter");

    KT_REGISTER_WRITER(KTJSONWriter, "json-writer");
    KT_REGISTER_PROCESSOR(KTJSONWriter, "json-writer");

    KTJSONWriter::KTJSONWriter(const std::string& name) :
            KTWriterWithTypists< KTJSONWriter, KTJSONTypeWriter >(name),
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

    bool KTJSONWriter::Configure(const scarab::param_node* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->get_value("output-file", fFilename));
            SetFileMode(node->get_value("file-mode", fFileMode));
            SetPrettyJSONFlag(node->get_value<bool>("pretty-json", fPrettyJSONFlag));
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
