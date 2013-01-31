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
            fUseTFile(true),
            fTFilename("multi_event.root"),
            fTFileFlag("recreate"),
            fUseGraphics(false),
            fGraphicsFilePath(),
            fGraphicsFilenameBase("multi_event"),
            fGraphicsFileType("png"),
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
            SetUseTFile(node->GetData<Bool_t>("use-tfile", fUseTFile));
            SetTFilename(node->GetData<string>("output-tfile", fTFilename));
            SetTFileFlag(node->GetData<string>("tfile-flag", fTFileFlag));

            SetUseGraphics(node->GetData<Bool_t>("use-graphics", fUseGraphics));
            SetGraphicsFilePath(node->GetData<string>("graphics-file-path", fGraphicsFilePath));
            SetGraphicsFilenameBase(node->GetData<string>("graphics-filename-base", fGraphicsFilenameBase));
            SetGraphicsFileType(node->GetData<string>("graphics-file-type", fGraphicsFileType));
        }

        return true;
    }

    Bool_t KTMultiEventROOTWriter::OpenAndVerifyFile()
    {
        if (fUseTFile)
        {
            if (fFile == NULL)
            {
                fFile = new TFile(fTFilename.c_str(), fTFileFlag.c_str());
            }
            if (! fFile->IsOpen())
            {
                delete fFile;
                fFile = NULL;
                KTERROR(publog, "Output file <" << fTFilename << "> did not open!");
                return false;
            }
            fFile->cd();
        }
        return true;
    }

    void KTMultiEventROOTWriter::Start()
    {
        for (TypeWriterMap::iterator it = fTypeWriters.begin(); it != fTypeWriters.end(); it++)
        {
            static_cast< KTMEROOTTypeWriterBase* >(it->second)->StartNewHistograms();
        }
        return;
    }

    void KTMultiEventROOTWriter::Finish()
    {
        for (TypeWriterMap::iterator it = fTypeWriters.begin(); it != fTypeWriters.end(); it++)
        {
            static_cast< KTMEROOTTypeWriterBase* >(it->second)->FinishHistograms();
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
