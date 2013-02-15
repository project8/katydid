/*
 * KTMultiBundleROOTWriter.cc
 *
 *  Created on: Jan 28, 2013
 *      Author: nsoblath
 */

#include "KTMultiBundleROOTWriter.hh"

#include "KTFactory.hh"
#include "KTLogger.hh"
#include "KTPStoreNode.hh"
#include "KTWriteableData.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "katydid.output");


    static KTDerivedRegistrar< KTWriter, KTMultiBundleROOTWriter > sMERWriterRegistrar("multi-bundle-root-writer");
    static KTDerivedRegistrar< KTProcessor, KTMultiBundleROOTWriter > sMERWProcRegistrar("multi-bundle-root-writer");

    KTMultiBundleROOTWriter::KTMultiBundleROOTWriter() :
            KTWriterWithTypists< KTMultiBundleROOTWriter >(),
            fUseTFile(true),
            fTFilename("multi_bundle.root"),
            fTFileFlag("recreate"),
            fUseGraphics(false),
            fGraphicsFilePath(),
            fGraphicsFilenameBase("multi_bundle"),
            fGraphicsFileType("png"),
            fFile(NULL)
    {
        fConfigName = "multi-bundle-root-writer";

        RegisterSlot("start", this, &KTMultiBundleROOTWriter::Start, "void ()");
        RegisterSlot("finish", this, &KTMultiBundleROOTWriter::Finish, "void ()");


        RegisterSlot("write-data", this, &KTMultiBundleROOTWriter::Publish);
    }

    KTMultiBundleROOTWriter::~KTMultiBundleROOTWriter()
    {
        if (fFile != NULL)
        {
            fFile->Close();
        }
        delete fFile;
    }

    Bool_t KTMultiBundleROOTWriter::Configure(const KTPStoreNode* node)
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

    Bool_t KTMultiBundleROOTWriter::OpenAndVerifyFile()
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

    void KTMultiBundleROOTWriter::Start()
    {
        for (TypeWriterMap::iterator it = fTypeWriters.begin(); it != fTypeWriters.end(); it++)
        {
            static_cast< KTMEROOTTypeWriterBase* >(it->second)->StartNewHistograms();
        }
        return;
    }

    void KTMultiBundleROOTWriter::Finish()
    {
        for (TypeWriterMap::iterator it = fTypeWriters.begin(); it != fTypeWriters.end(); it++)
        {
            static_cast< KTMEROOTTypeWriterBase* >(it->second)->FinishHistograms();
        }
        return;
    }


} /* namespace Katydid */
