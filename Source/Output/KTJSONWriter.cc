/*
 * KTJSONWriter.cc
 *
 *  Created on: Aug 24, 2012
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
            fFileFlag("recreate")
            //fFile(NULL)
    {
        fConfigName = "basic-root-writer";

        RegisterSlot("write-data", this, &KTJSONWriter::Publish);
    }

    KTJSONWriter::~KTJSONWriter()
    {
        /*
        if (fFile != NULL)
        {
            fFile->Close();
        }
        delete fFile;
        */
    }

    Bool_t KTJSONWriter::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetData<string>("output-file", fFilename));
            SetFileFlag(node->GetData<string>("file-flag", fFileFlag));
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
