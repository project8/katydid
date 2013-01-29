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

#include "TFile.h"
#include "TTree.h"

using std::set;
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
            fFile(NULL),
            fTrees()
    {
        fConfigName = "root-tree-writer";

        RegisterSlot("write-data", this, &KTROOTTreeWriter::Publish);
    }

    KTROOTTreeWriter::~KTROOTTreeWriter()
    {
        if (fFile != NULL)
        {
            CloseFile();
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

    void KTROOTTreeWriter::AddTree(TTree* newTree)
    {
        newTree->SetDirectory(fFile);
        fTrees.insert(newTree);
        return;
    }


    TFile* KTROOTTreeWriter::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        fFile = new TFile(filename.c_str(), flag.c_str());
        return fFile;
    }

    void KTROOTTreeWriter::WriteTrees()
    {
        KTWARN(publog, "writing trees");
        fFile->cd();
        for (set< TTree* >::iterator it = fTrees.begin(); it != fTrees.end(); it++)
        {
            KTWARN(publog, "Tree being written has " << (*it)->GetEntries() << " entries");
            (*it)->Write();
        }
        fFile->Write();
        return;
    }

    void KTROOTTreeWriter::CloseFile()
    {
        if (fFile != NULL)
        {
            WriteTrees();
            fTrees.clear();

            fFile->Close();
            delete fFile;
            fFile = NULL;
        }
        return;
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
