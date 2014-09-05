/*
 * KTHDF5Writer.cc
 *
 *  Created on: Jan 23, 2013
 *      Author: nsoblath
 */

#include "KTHDF5Writer.hh"

#include "KTCommandLineOption.hh"
#include "KTParam.hh"

#include "TFile.h"
#include "TTree.h"

using std::set;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTHDF5Writer");


    KT_REGISTER_WRITER(KTHDF5Writer, "hdf5-writer");
    KT_REGISTER_PROCESSOR(KTHDF5Writer, "hdf5-writer");

    static KTCommandLineOption< string > sRTWFilenameCLO("HDF5 Writer", "HDF5 writer filename", "hdf5-file");

    KTHDF5Writer::KTHDF5Writer(const std::string& name) :
            KTWriterWithTypists< KTHDF5Writer >(name),
            fFilename("my_file.h5")
    {
        RegisterSlot("close-file", this, &KTHDF5Writer::CloseFile);
    }

    KTHDF5Writer::~KTHDF5Writer()
    {
        if (true /*obviously this should be replaced by a condition checking if the file is open*/)
        {
            CloseFile();
        }
    }

    bool KTHDF5Writer::Configure(const KTParamNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetValue("output-file", fFilename));
        }

        // Command-line settings
        SetFilename(fCLHandler->GetCommandLineValue< string >("rtw-file", fFilename));

        return true;
    }

    bool KTHDF5Writer::OpenAndVerifyFile()
    {
        return true;
    }

    bool KTHDF5Writer::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();

        return true;
    }

    void KTHDF5Writer::CloseFile()
    {
        if (true)
        {
            KTINFO(publog, "HDF5 data written to file <" << "blah" << ">; closing file");

        }
        return;
    }

} /* namespace Katydid */
