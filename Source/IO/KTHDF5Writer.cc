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
            fFilename("my_file.h5"),
            fFile(NULL)
    {
        RegisterSlot("close-file", this, &KTHDF5Writer::CloseFile);
        RegisterSlot("write-header", this, &KTHDF5Writer::WriteEggHeader);
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

    void KTHDF5Writer::WriteEggHeader(KTEggHeader* header) {
        // TODO(kofron): clearly this is insufficient
        // TODO(kofron): H5T_VARIABLE length for strings?
        if (!this->OpenAndVerifyFile()) return;

        H5::Group* header_grp = this->AddGroup("/metadata");
       
        // Write the timestamp
        std::string string_attr = header->GetTimestamp();
        H5::StrType ts_type(H5::PredType::C_S1, string_attr.length() + 1);
        H5::DataSpace attr_space(H5S_SCALAR);
        H5::Attribute attr = header_grp->createAttribute("timestamp",
                                                        ts_type,
                                                        attr_space);
        attr.write(ts_type, string_attr.c_str());

        // Write the header.
        // TODO: this should go in a separate group called 'header' under
        //       metadata.
        this->AddMetadata("description", header->GetDescription());
        this->AddMetadata("acquisition_mode", header->GetAcquisitionMode());
        this->AddMetadata("n_channels", header->GetNChannels());        
        this->AddMetadata("raw_slice_size",header->GetRawSliceSize());
        this->AddMetadata("slice_size",header->GetSliceSize());
        this->AddMetadata("slice_stride",header->GetSliceStride());
        this->AddMetadata("record_size",header->GetRecordSize());
        this->AddMetadata("run_duration",header->GetRunDuration());
        this->AddMetadata("acquisition_rate",header->GetAcquisitionRate());
        this->AddMetadata("run_type",header->GetRunType());
        this->AddMetadata("format_mode",header->GetFormatMode());
        this->AddMetadata("data_type_size",header->GetDataTypeSize());
        this->AddMetadata("bit_depth",header->GetBitDepth());
        this->AddMetadata("voltage_min",header->GetVoltageMin());
        this->AddMetadata("voltage_range",header->GetVoltageRange());
        return;
    }

    void KTHDF5Writer::WriteMetadata(std::string name, H5::DataType type, const void* value) {
        H5::DataSpace dspace(H5S_SCALAR);
        H5::Group* grp = this->AddGroup("/metadata");
        H5::DSetCreatPropList plist;
        H5::DataSet* dset = new H5::DataSet(grp->createDataSet(name.c_str(),
                                                               type,
                                                               dspace,
                                                               plist));
        dset->write(value, type);
    }

} /* namespace Katydid */
