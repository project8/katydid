/*
 * KTBasicROOTFileWriter.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTBasicROOTFileWriter.hh"

#include "KTROOTWriterFileManager.hh"

#include "KTCommandLineOption.hh"

//#include "param.hh"
#include "path.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTBasicROOTFileWriter");


    KT_REGISTER_WRITER(KTBasicROOTFileWriter, "basic-root-writer");
    KT_REGISTER_PROCESSOR(KTBasicROOTFileWriter, "basic-root-writer");

    static Nymph::KTCommandLineOption< string > sBRWFilenameCLO("Basic ROOT File Writer", "Basic ROOT file writer filename", "brw-file");

    KTBasicROOTFileWriter::KTBasicROOTFileWriter(const std::string& name) :
            KTWriterWithTypists< KTBasicROOTFileWriter, KTBasicROOTTypeWriter >(name),
            fFilename("basic_output.root"),
            fFileFlag("recreate"),
            fFile(NULL),
            fFileManager(KTROOTWriterFileManager::get_instance())
    {
    }

    KTBasicROOTFileWriter::~KTBasicROOTFileWriter()
    {
        CloseFile();
    }

    bool KTBasicROOTFileWriter::Configure(const scarab::param_node* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->get_value("output-file", fFilename));
            SetFileFlag(node->get_value("file-flag", fFileFlag));
        }

        // Command-line settings
        SetFilename(fCLHandler->GetCommandLineValue< string >("brw-file", fFilename));

        return true;
    }

    TFile* KTBasicROOTFileWriter::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        fFile = fFileManager->OpenFile(this, filename.c_str(), flag.c_str());
        return fFile;
    }

    void KTBasicROOTFileWriter::CloseFile()
    {
        if (fFile != NULL)
        {
            fFileManager->FinishFile(this, fFilename);
            fFile = NULL;
        }
        return;
    }

    bool KTBasicROOTFileWriter::OpenAndVerifyFile()
    {
        if (fFile == NULL)
        {
            fFile = fFileManager->OpenFile(this, fFilename.c_str(), fFileFlag.c_str());
        }
        if (! fFile->IsOpen())
        {
            fFileManager->DiscardFile(this, fFilename);
            fFile = NULL;
            KTERROR(publog, "Output file <" << fFilename << "> did not open!");
            return false;
        }
        fFile->cd();
        return true;
    }

} /* namespace Katydid */
