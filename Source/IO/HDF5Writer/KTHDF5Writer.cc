/*
 * KTHDF5Writer.cc
 *
 *  Created on: Sept 5, 2014
 *      Author: J.N. Kofron, N.S. Oblath
 */

#include "KTHDF5Writer.hh"

#include "KTCommandLineOption.hh"

#include "path.hh"

using std::set;
using std::string;

namespace Katydid
{
    KTLOGGER(publog, "KTHDF5Writer");


    KT_REGISTER_WRITER(KTHDF5Writer, "hdf5-writer");
    KT_REGISTER_PROCESSOR(KTHDF5Writer, "hdf5-writer");

    static Nymph::KTCommandLineOption< string > sRTWFilenameCLO("HDF5 Writer", "HDF5 writer filename", "hdf5-file");

    KTHDF5Writer::KTHDF5Writer(const std::string& name) :
            KTWriterWithTypists< KTHDF5Writer, KTHDF5TypeWriter >(name),
            fHeaderSlot("header", this, &KTHDF5Writer::WriteEggHeader),
            fFilename("my_file.h5"),
            fUseCompressionFlag(false),
            fFile(NULL),
            fHeaderParsed(false)
    {
        RegisterSlot("close-file", this, &KTHDF5Writer::CloseFile);
    }

    KTHDF5Writer::~KTHDF5Writer()
    {
            CloseFile();
    }

    bool KTHDF5Writer::Configure(const scarab::param_node* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->get_value("output-file", fFilename));
            SetUseCompressionFlag(node->get_value("use-compression", fUseCompressionFlag));
        }

        // Command-line settings
        SetFilename(fCLHandler->GetCommandLineValue< string >("hdf5-file", fFilename));

        return true;
    }

    bool KTHDF5Writer::OpenAndVerifyFile()
    {
        if (fFile == NULL) {
            fFile = OpenFile(fFilename);
        }
        if (!fFile) {
            delete fFile;
            fFile = NULL;
            KTERROR(publog, "Error opening HDF5 file <" << fFilename << ">");
            return false;
        }
        return true;
    }

    H5::H5File* KTHDF5Writer::OpenFile(const std::string& filename)
    {
        CloseFile();

        // determine the absolute path and parent directory
        scarab::path absFilepath = scarab::expand_path(filename);
        scarab::path fileDir = absFilepath.parent_path();

        if (! scarab::fs::is_directory(fileDir))
        {
            KTERROR(publog, "Parent directory of output file <" << fileDir << "> does not exist or is not a directory");
            return nullptr;
        }

        KTDEBUG(publog, "Opening file <" << absFilepath << ">");

        this->fFile = new H5::H5File(absFilepath.c_str(), H5F_ACC_TRUNC);
        KTINFO(publog, "opened HDF5 file <" << absFilepath << ">");
        return this->fFile;
    }

    void KTHDF5Writer::CloseFile()
    {
        if (fFile != NULL) {
            KTINFO(publog, "HDF5 data written to file <" << this->fFilename << ">; closing file");
            delete fFile;
            fFile = NULL;
        }
        return;
    }

    H5::Group* KTHDF5Writer::AddGroup(const std::string& groupname)
    {
        std::map<std::string, H5::Group*>::iterator it;
        H5::Group* result;
        if ( (it = this->fGroups.find(groupname)) == this->fGroups.end() )
        {
            result = new H5::Group(fFile->createGroup(groupname));
            this->fGroups[groupname] = result;
        }
        else
        {
            result = it->second;
        }
        return result;
    }

    bool KTHDF5Writer::WriteEggHeader(KTEggHeader& header)
    {
        // TODO(kofron): clearly this is insufficient
        // TODO(kofron): H5T_VARIABLE length for strings?
        if (!this->OpenAndVerifyFile()) return false;

        // copy the egg header.
        this->fHeader = header;
        this->fHeader.SetChannelHeader(header.GetChannelHeader(0),0);
        this->fHeaderParsed = true;


        //H5::Group* header_grp = this->AddGroup("/metadata");

        // Write the header.
        this->AddScalar("egg-header/mantis_timestamp",this->fHeader.GetTimestamp());
        this->AddScalar("egg-header/description", this->fHeader.GetDescription());
        this->AddScalar("egg-header/acquisition_mode", this->fHeader.GetAcquisitionMode());
        this->AddScalar("egg-header/n_channels", this->fHeader.GetNChannels());
        this->AddScalar("egg-header/raw_slice_size",this->fHeader.GetChannelHeader(0)->GetRawSliceSize()); // in number of bins
        this->AddScalar("egg-header/slice_size",this->fHeader.GetChannelHeader(0)->GetSliceSize()); // in number of bins
        this->AddScalar("egg-header/slice_stride",this->fHeader.GetChannelHeader(0)->GetSliceStride()); // in number of bins
        this->AddScalar("egg-header/record_size",this->fHeader.GetChannelHeader(0)->GetRecordSize()); // in number of bins
        this->AddScalar("egg-header/run_duration",this->fHeader.GetRunDuration()); // in ms
        this->AddScalar("egg-header/acquisition_rate",this->fHeader.GetAcquisitionRate()); // in Hz
        this->AddScalar("egg-header/data_type_size",this->fHeader.GetChannelHeader(0)->GetDataTypeSize());
        this->AddScalar("egg-header/bit_depth",this->fHeader.GetChannelHeader(0)->GetBitDepth());
        this->AddScalar("egg-header/voltage_offset",this->fHeader.GetChannelHeader(0)->GetVoltageOffset());
        this->AddScalar("egg-header/voltage_range",this->fHeader.GetChannelHeader(0)->GetVoltageRange());
        this->AddScalar("egg-header/center_frequency",this->fHeader.GetCenterFrequency()); // in Hz
        this->AddScalar("egg-header/minimum_frequency",this->fHeader.GetMinimumFrequency()); // in Hz
        this->AddScalar("egg-header/maximum_frequency",this->fHeader.GetMaximumFrequency()); // in Hz
        return true;
    }

    void KTHDF5Writer::WriteScalar(std::string name, H5::DataType type, const void* value) {
        std::string group_name;
        std::stringstream group_name_builder;
        group_name_builder << "/";
        H5::CommonFG* grp = fFile; // *(this->AddGroup(group_name));
        std::string delimiter = "/";
        size_t pos = 0;
        std::string token;

        if (name.at(0) == '/')
        {
            name.erase(0, 1);
        }
        while ((pos = name.find(delimiter)) != std::string::npos)
        {
            token = name.substr(0, pos);
            group_name_builder << token;
            group_name_builder >> group_name;
            grp = this->AddGroup(group_name);
            name.erase(0, pos + delimiter.length());
            KTINFO(group_name + delimiter + name);
        }
        H5::DataSpace dspace(H5S_SCALAR);
        H5::DSetCreatPropList plist;
        H5::DataSet* dset = new H5::DataSet(grp->createDataSet(name.c_str(),
                                                               type,
                                                               dspace,
                                                               plist));
        dset->write(value, type);
    }

    bool KTHDF5Writer::DidParseHeader()
    {
        return this->fHeaderParsed;
    }

    KTEggHeader* KTHDF5Writer::GetHeader()
    {
        return &(this->fHeader);
    }

} /* namespace Katydid */
