/*
 * KTROOTTreeWriter.cc
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeWriter.hh"

#include "KTCommandLineOption.hh"
#include "KTParam.hh"

#include "TFile.h"
#include "TTree.h"

using std::set;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTROOTTreeWriter");


    KT_REGISTER_WRITER(KTROOTTreeWriter, "root-tree-writer");
    KT_REGISTER_PROCESSOR(KTROOTTreeWriter, "root-tree-writer");

    static KTCommandLineOption< string > sRTWFilenameCLO("ROOT Tree Writer", "ROOT Tree writer filename", "rtw-file");

    KTROOTTreeWriter::KTROOTTreeWriter(const std::string& name) :
            KTWriterWithTypists< KTROOTTreeWriter, KTROOTTreeTypeWriter >(name),
            fFilename("tree_output.root"),
            fFileFlag("recreate"),
            fAccumulate(false),
            fFile(NULL),
            fTrees()
    {
        RegisterSlot("close-file", this, &KTROOTTreeWriter::CloseFile);
    }

    KTROOTTreeWriter::~KTROOTTreeWriter()
    {
        if (fFile != NULL)
        {
            CloseFile();
        }
        delete fFile;
    }

    bool KTROOTTreeWriter::Configure(const KTParamNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetValue("output-file", fFilename));
            SetFileFlag(node->GetValue("file-flag", fFileFlag));
            SetAccumulate(node->GetValue("accumulate", fAccumulate));
        }

        // Command-line settings
        SetFilename(fCLHandler->GetCommandLineValue< string >("rtw-file", fFilename));

        return true;
    }

    bool KTROOTTreeWriter::OpenAndVerifyFile()
    {
        if (fFile == NULL)
        {
            KTINFO(publog, "Opening ROOT file <" << fFilename << "> with file flag <" << fFileFlag << ">");
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
        if (fFile == NULL || ! fFile->IsOpen())
        {
            KTWARN(publog, "Attempt made to write trees, but the file is not open");
            return;
        }
        KTINFO(publog, "Writing trees");
        fFile->cd();
        for (set< TTree* >::iterator it = fTrees.begin(); it != fTrees.end(); it++)
        {
            if (*it != NULL)
            {
                KTINFO(publog, "Tree <" << (*it)->GetName() << "> has " << (*it)->GetEntries() << " entries");
                (*it)->Write();
            }
        }

        if( fAccumulate )
        {
            fFile->Purge();
        }

        fTrees.clear();
        return;
    }

    void KTROOTTreeWriter::CloseFile()
    {
        if (fFile != NULL)
        {
            WriteTrees();

            KTINFO(publog, "Trees written to file <" << fFile->GetName() << ">; closing file");

            fFile->Close();
            delete fFile;
            fFile = NULL;
        }
        return;
    }

} /* namespace Katydid */