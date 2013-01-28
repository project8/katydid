/*
 * KTMultiEventROOTWriter.cc
 *
 *  Created on: Jan 28, 2013
 *      Author: nsoblath
 */

#include "KTMultiEventROOTWriter.hh"

#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTWriteableData.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    static KTDerivedRegistrar< KTWriter, KTMultiEventROOTWriter > sMERWriterRegistrar("multi-event-root-writer");
    static KTDerivedRegistrar< KTProcessor, KTMultiEventROOTWriter > sMERWProcRegistrar("multi-event-root-writer");

    KTMultiEventROOTWriter::KTMultiEventROOTWriter() :
            KTWriterWithTypists< KTMultiEventROOTWriter >(),
            fFilename("basic_output.root"),
            fFileFlag("recreate"),
            fFile(NULL)
    {
        fConfigName = "multi-event-root-writer";

        RegisterSlot("start", this, &KTMultiEventROOTWriter::Start, "void ()");
        RegisterSlot("finish", this, &KTMultiEventROOTWriter::Finish, "void ()");


        RegisterSlot("write-data", this, &KTMultiEventROOTWriter::Publish);
    }

    KTMultiEventROOTWriter::~KTMultiEventROOTWriter()
    {
        if (fFile != NULL)
        {
            fFile->Close();
        }
        delete fFile;
    }

    Bool_t KTMultiEventROOTWriter::Configure(const KTPStoreNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetData<string>("output-file", fFilename));
            SetFileFlag(node->GetData<string>("file-flag", fFileFlag));
        }

        return true;
    }

    Bool_t KTMultiEventROOTWriter::OpenAndVerifyFile()
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

    void KTMultiEventROOTWriter::Start()
    {
        for (TypeWriterMap::iterator it = fTypeWriters.begin(); it != fTypeWriters.end(); it++)
        {
            static_cast< KTMultiEventROOTTypeWriter* >(it->second)->StartNewHistograms();
        }
        return;
    }

    void KTMultiEventROOTWriter::Finish()
    {
        for (TypeWriterMap::iterator it = fTypeWriters.begin(); it != fTypeWriters.end(); it++)
        {
            static_cast< KTMultiEventROOTTypeWriter* >(it->second)->FinishHistograms();
        }
        return;
    }


    void KTMultiEventROOTWriter::Publish(const KTWriteableData* data)
    {
        data->Accept(this);
        return;
    }

    void KTMultiEventROOTWriter::Write(const KTWriteableData* data)
    {
        KTWARN(publog, "Generic Write function called; no data written");
        return;
    }

} /* namespace Katydid */
