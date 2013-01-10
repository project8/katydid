/*
 * KTJSONWriter.cc
 *
 *  Created on: Jan 3, 2013
 *      Author: nsoblath
 */

#include "KTJSONWriter.hh"

#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTWriteableData.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");

    static KTDerivedRegistrar< KTWriter, KTJSONWriter > sBRFWriterRegistrar("json-writer");
    static KTDerivedRegistrar< KTProcessor, KTJSONWriter > sBRFWProcRegistrar("json-writer");

    KTJSONWriter::KTJSONWriter() :
            KTWriterWithTypists< KTJSONWriter >(),
            fFilename("basic_output.root"),
            fFileMode("w+"),
            fPrettyJSONFlag(true),
            fFile(NULL),
            fFileStream(NULL),
            fJSONMaker(NULL)
    {
        fConfigName = "json-writer";

        RegisterSlot("write-data", this, &KTJSONWriter::Publish);
    }

    KTJSONWriter::~KTJSONWriter()
    {
        CloseFile();
    }

    Bool_t KTJSONWriter::Configure(const KTPStoreNode* node)
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

    Bool_t KTJSONWriter::OpenFile()
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

    Bool_t KTJSONWriter::OpenAndVerifyFile()
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

    void KTJSONWriter::Publish(const KTWriteableData* data)
    {
        data->Accept(this);
        return;
    }

    void KTJSONWriter::Write(const KTWriteableData* data)
    {
        KTWARN(publog, "Generic Write function called; no data written");
        return;
    }

} /* namespace Katydid */
