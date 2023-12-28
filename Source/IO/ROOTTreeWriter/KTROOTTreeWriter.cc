/*
 * KTROOTTreeWriter.cc
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#include "KTROOTTreeWriter.hh"

#include "KTROOTWriterFileManager.hh"

#include "KTCommandLineOption.hh"

#include "TFile.h"
#include "TTree.h"

using std::set;
using std::string;

namespace Katydid
{
    LOGGER(publog, "KTROOTTreeWriter");


    KT_REGISTER_WRITER(KTROOTTreeWriter, "root-tree-writer");
    KT_REGISTER_PROCESSOR(KTROOTTreeWriter, "root-tree-writer");

    static Nymph::KTCommandLineOption< string > sRTWFilenameCLO("ROOT Tree Writer", "ROOT Tree writer filename", "rtw-file");

    KTROOTTreeWriter::KTROOTTreeWriter(const std::string& name) :
            KTWriterWithTypists< KTROOTTreeWriter, KTROOTTreeTypeWriter >(name),
            fFilename("tree_output.root"),
            fFileFlag("recreate"),
            fAccumulate(false),
            fFile(NULL),
            fFileManager(KTROOTWriterFileManager::get_instance()),
            fTrees()
    {
        RegisterSlot("close-file", this, &KTROOTTreeWriter::CloseFile);
    }

    KTROOTTreeWriter::~KTROOTTreeWriter()
    {
        CloseFile();
    }

    bool KTROOTTreeWriter::Configure(const scarab::param_node* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->get_value("output-file", fFilename));
            SetFileFlag(node->get_value("file-flag", fFileFlag));
            SetAccumulate(node->get_value("accumulate", fAccumulate));
        }

        // Command-line settings
        SetFilename(fCLHandler->GetCommandLineValue< string >("rtw-file", fFilename));

        return true;
    }

    bool KTROOTTreeWriter::OpenAndVerifyFile()
    {
        if (fFile == NULL)
        {
            LINFO(publog, "Opening ROOT file <" << fFilename << "> with file flag <" << fFileFlag << ">");
            fFile = fFileManager->OpenFile(this, fFilename.c_str(), fFileFlag.c_str());
        }
        if (! fFile->IsOpen())
        {
            fFileManager->DiscardFile(this, fFilename);
            fFile = NULL;
            LERROR(publog, "Output file <" << fFilename << "> did not open!");
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
        fFile = fFileManager->OpenFile(this, filename.c_str(), flag.c_str());
        return fFile;
    }

    void KTROOTTreeWriter::WriteTrees()
    {
        if (fFile == NULL || ! fFile->IsOpen())
        {
            LWARN(publog, "Attempt made to write trees, but the file is not open");
            return;
        }
        LINFO(publog, "Writing trees");
        fFile->cd();
        for (set< TTree* >::iterator it = fTrees.begin(); it != fTrees.end(); it++)
        {
            if (*it != NULL)
            {
                LINFO(publog, "Tree <" << (*it)->GetName() << "> has " << (*it)->GetEntries() << " entries");
                (*it)->Write();
            }
        }

        if( fAccumulate )
        {
            fFile->Purge();
        }

        fTrees.clear();

        LINFO(publog, "Trees written to file <" << fFile->GetName() << ">; closing file");
        return;
    }

    void KTROOTTreeWriter::CloseFile()
    {
        if (fFile != NULL)
        {
            WriteTrees();

            fFileManager->FinishFile(this, fFilename);
            fFile = NULL;
        }
        return;
    }

} /* namespace Katydid */
