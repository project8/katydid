/*
 * KTHDF5Writer.cc
 *
 *  Created on: Sept 5, 2014
 *      Author: J.N. Kofron, N.S. Oblath
 */

#include "KTHDF5Writer.hh"

#include "KTCommandLineOption.hh"
#include "KTParam.hh"

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
            fHeaderSlot("header", this, &KTHDF5Writer::WriteEggHeader),
            fFilename("my_file.h5"),
            fFileFlag("nocompress"),
            fFile(NULL),
            fHeaderParsed(false)
    {
        RegisterSlot("close-file", this, &KTHDF5Writer::CloseFile);
    }

    KTHDF5Writer::~KTHDF5Writer()
    {
            CloseFile();
    }

    bool KTHDF5Writer::Configure(const KTParamNode* node)
    {
        // Config-file settings
        if (node != NULL)
        {
            SetFilename(node->GetValue("output-file", fFilename));
            SetFileFlag(node->GetValue("file-flag", fFileFlag));
        }

        // Command-line settings
        SetFilename(fCLHandler->GetCommandLineValue< string >("hdf5-file", fFilename));

        return true;
    }

    bool KTHDF5Writer::OpenAndVerifyFile()
    {
        if (fFile == NULL) {
            KTDEBUG(publog, "Opening HDF5 file");
            fFile = new H5::H5File(fFilename.c_str(), H5F_ACC_TRUNC);
            KTDEBUG(publog, "Done.");
        }
        if (!fFile) {
            delete fFile;
            fFile = NULL;
            KTERROR(publog, "Error opening HDF5 file!!");
            return false;
        }
        return true;
    }

    H5::H5File* KTHDF5Writer::OpenFile(const std::string& filename, const std::string& flag)
    {
        CloseFile();
        this->fFile = new H5::H5File(filename.c_str(), H5F_ACC_TRUNC);
        KTINFO(publog, "opened HDF5 file!");
        return this->fFile;
    }

    void KTHDF5Writer::CloseFile()
    {
        if (fFile != NULL) {
            KTINFO(publog, 
                    "HDF5 data written to file <" 
                    << this->fFilename 
                    << ">; closing file");
            delete fFile;
            fFile = NULL;
        }
        return;
    }

    H5::Group* KTHDF5Writer::AddGroup(const std::string& groupname) {
        std::map<std::string, H5::Group*>::iterator it;
        H5::Group* result;
        if ( (it = this->fGroups.find(groupname)) == this->fGroups.end() ) {
            result = new H5::Group(fFile->createGroup(groupname));
            this->fGroups[groupname] = result;
        }
        else {
            result = it->second;
        }
        return result;
    }

    bool KTHDF5Writer::WriteEggHeader(KTEggHeader& header) {
        // TODO(kofron): clearly this is insufficient
        // TODO(kofron): H5T_VARIABLE length for strings?
        if (!this->OpenAndVerifyFile()) return false;

        // copy the egg header.
        this->fHeader = header;
        this->fHeader.SetChannelHeader(header.GetChannelHeader(0),0);
        KTDEBUG(publog, "Header Number of Channels: " << header.GetNChannels());
        this->fHeaderParsed = true;
        //KTDEBUG(publog, "header.GetChannelHeader(0): - " << header.GetChannelHeader(0));
        //KTDEBUG(publog, "this->fHeader.GetChannelHeader(0): - " << this->fHeader.GetChannelHeader(0));


        H5::Group* header_grp = this->AddGroup("/metadata");
       
        // Write the header.
        this->AddMetadata("header/mantis_timestamp",this->fHeader.GetTimestamp());
        this->AddMetadata("header/description", this->fHeader.GetDescription());
        this->AddMetadata("header/acquisition_mode", this->fHeader.GetAcquisitionMode());
        this->AddMetadata("header/n_channels", this->fHeader.GetNChannels());        
        this->AddMetadata("header/raw_slice_size",this->fHeader.GetChannelHeader(0)->GetRawSliceSize());
        this->AddMetadata("header/slice_size",this->fHeader.GetChannelHeader(0)->GetSliceSize());
        this->AddMetadata("header/slice_stride",this->fHeader.GetChannelHeader(0)->GetSliceStride());
        this->AddMetadata("header/record_size",this->fHeader.GetChannelHeader(0)->GetRecordSize());
        this->AddMetadata("header/run_duration",this->fHeader.GetRunDuration());
        this->AddMetadata("header/acquisition_rate",this->fHeader.GetAcquisitionRate());
        this->AddMetadata("header/data_type_size",this->fHeader.GetChannelHeader(0)->GetDataTypeSize());
        this->AddMetadata("header/bit_depth",this->fHeader.GetChannelHeader(0)->GetBitDepth());
        this->AddMetadata("header/voltage_offset",this->fHeader.GetChannelHeader(0)->GetVoltageOffset());
        this->AddMetadata("header/voltage_range",this->fHeader.GetChannelHeader(0)->GetVoltageRange());
        this->AddMetadata("header/center_frequency",this->fHeader.GetCenterFrequency());
        this->AddMetadata("header/minimum_frequency",this->fHeader.GetMinimumFrequency());
        this->AddMetadata("header/maximum_frequency",this->fHeader.GetMaximumFrequency());
        return true;
    }

    void KTHDF5Writer::WriteMetadata(std::string name, H5::DataType type, const void* value) {
        std::string group_name = "/metadata";
        std::stringstream group_name_builder;
        group_name_builder << group_name << "/";
        H5::Group grp = *(this->AddGroup(group_name));
        std::string delimiter = "/";
        size_t pos = 0;
        std::string token;

        if (name.at(0) == '/') {
            name.erase(0, 1);
        }
        while ((pos = name.find(delimiter)) != std::string::npos) {
            token = name.substr(0, pos);
            group_name_builder << token;
            group_name_builder >> group_name;
            grp = *(this->AddGroup(group_name));
            name.erase(0, pos + delimiter.length());
            KTINFO(group_name + delimiter + name);
        }
        H5::DataSpace dspace(H5S_SCALAR);
        H5::DSetCreatPropList plist;
        H5::DataSet* dset = new H5::DataSet(grp.createDataSet(name.c_str(),
                                                               type,
                                                               dspace,
                                                               plist));
        dset->write(value, type);
    }

    bool KTHDF5Writer::DidParseHeader() {
        return this->fHeaderParsed;
    }

    KTEggHeader* KTHDF5Writer::GetHeader() {
        return &(this->fHeader);
    }

} /* namespace Katydid */
