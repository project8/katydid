/*
 * KTBasicROOTFileWriter.cc
 *
 *  Created on: Aug 24, 2012
 *      Author: nsoblath
 */

#include "KTBasicROOTFileWriter.hh"

#include "KTCommandLineOption.hh"
//#include "param.hh"

using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTBasicROOTFileWriter");


    KT_REGISTER_WRITER(KTBasicROOTFileWriter, "basic-root-writer");
    KT_REGISTER_PROCESSOR(KTBasicROOTFileWriter, "basic-root-writer");

    static KTCommandLineOption< string > sBRWFilenameCLO("Basic ROOT File Writer", "Basic ROOT file writer filename", "brw-file");

    KTBasicROOTFileWriter::KTBasicROOTFileWriter(const std::string& name) :
            KTWriterWithTypists< KTBasicROOTFileWriter, KTBasicROOTTypeWriter >(name),
            fFilename("basic_output.root"),
            fFileFlag("recreate"),
            fFile(NULL)
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

    bool KTBasicROOTFileWriter::OpenAndVerifyFile()
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

} /* namespace Katydid */
