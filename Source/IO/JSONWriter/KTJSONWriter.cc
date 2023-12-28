/*
 * KTJSONWriter.cc
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#include "KTJSONWriter.hh"

#include "param.hh"
#include "param_json.hh" // for JSON_FILE_BUFFER_SIZE


using std::string;

namespace Katydid
{
    LOGGER(publog, "KTJSONWriter");

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

        char tBuffer[ RAPIDJSON_FILE_BUFFER_SIZE ];
        if (fFilename == "stdout")
        {
            fFileStream = new rapidjson::FileWriteStream(stdout, tBuffer, sizeof(tBuffer));
        }
        else
        {
            fFile = fopen(fFilename.c_str(), fFileMode.c_str());
            if (fFile == NULL)
            {
                LERROR(publog, "File did not open\n" <<
                        "\tFilename: " << fFilename <<
                        "\tMode: " << fFileMode);
                return false;
            }
            fFileStream = new rapidjson::FileWriteStream(fFile, tBuffer, sizeof(tBuffer));
        }

        if (fPrettyJSONFlag)
        {
            fJSONMaker = new KTJSONMakerPretty< rapidjson::FileWriteStream >(*fFileStream);
        }
        else
        {
            fJSONMaker = new KTJSONMakerCompact< rapidjson::FileWriteStream >(*fFileStream);
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
