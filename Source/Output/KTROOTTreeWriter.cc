/*
 * KTROOTTreeWriter.cc
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeWriter.hh"

#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTWriteableData.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    static KTDerivedRegistrar< KTWriter, KTROOTTreeWriter > sRTWriterRegistrar("root-tree-writer");
    static KTDerivedRegistrar< KTProcessor, KTROOTTreeWriter > sRTWProcRegistrar("root-tree-writer");

    KTROOTTreeWriter::KTROOTTreeWriter() :
            KTWriterWithTypists< KTROOTTreeWriter >(),
            fFilename("tree_output.root"),
            fFileFlag("recreate"),
            fFile(NULL)
    {
        fConfigName = "root-tree-writer";

        RegisterSlot("write-data", this, &KTROOTTreeWriter::Publish);
    }

    KTROOTTreeWriter::~KTROOTTreeWriter()
    {
        if (fFile != NULL)
        {
            fFile->Close();
        }
        delete fFile;
    }

    Bool_t KTROOTTreeWriter::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetData<string>("output-file", fFilename));
            SetFileFlag(node->GetData<string>("file-flag", fFileFlag));
        }

        return true;
    }

    Bool_t KTROOTTreeWriter::OpenAndVerifyFile()
    {
        if (fFile == NULL)
        {
            fFile = new TFile(fFilename.c_str(), fFileFlag.c_str());
        }
        if (! fFile->IsOpen())
        {
            delete fFile;
            fFile = NULL;
            KTERROR(publog, "Output file <" << fFilename << "> did not open!");
            return false;
        }
        fFile->cd();
        return true;
    }

    void KTROOTTreeWriter::Publish(const KTWriteableData* data)
    {
        data->Accept(this);
        return;
    }

    void KTROOTTreeWriter::Write(const KTWriteableData* data)
    {
        KTWARN(publog, "Generic Write function called; no data written");
        return;
    }

} /* namespace Katydid */
